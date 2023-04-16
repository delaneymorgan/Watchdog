//
// Created by craig on 6/04/23.
//

#include "Watchdog.h"

#include <iostream>
#include <csignal>

#include <boost/filesystem.hpp>
#include <boost/thread/thread.hpp>
#include <boost/interprocess/shared_memory_object.hpp>


#include "Heartbeat.h"


using namespace boost::filesystem;

namespace {
    const boost::chrono::milliseconds MIN_SCAN_PERIOD(10);
    const std::string HEARTBEATS_DIR = "/dev/shm/";
    const std::string SHARED_MEMORY_PATH = "/dev/shm";
    typedef std::map<std::string, boost::shared_ptr<EKG> >::iterator THeartbeatIterator;

    struct TProcInfo {
        pid_t m_ProcID;
        std::string m_ActualName;

        TProcInfo() :
                m_ProcID(0),
                m_ActualName() {
        }
    };

}


Watchdog::Watchdog(boost::chrono::milliseconds scanPeriod, bool autoScan, bool verbose) :
        m_Running(true),
        m_AutoScan(autoScan),
        m_Verbose(verbose),
        m_ScanPeriod(scanPeriod) {
}


Watchdog::~Watchdog() {
}


void Watchdog::monitor() {
    while (m_Running) {
        scanHeartbeats();
        boost::chrono::milliseconds smallestHB = m_ScanPeriod;
        for (THeartbeatIterator it = m_Heartbeats.begin(); it != m_Heartbeats.end(); it++) {
            boost::shared_ptr<EKG> ekg = it->second;
            try {
                boost::chrono::milliseconds normalLimit = ekg->normalLimit();
                if (normalLimit < smallestHB) {
                    smallestHB = normalLimit;
                }
                std::string isAlive = ekg->isAlive() ? "alive" : "dead";
                std::string isNormal = ekg->isNormal() ? "normal" : "warning";
                if (!ekg->isAlive()) {
                    m_CallBack(ekg->actualName(), ekg->processID(), ekg->threadID(), Fatal_HeartbeatEvent,
                               ekg->length(), m_Verbose);
                } else if (!ekg->isNormal()) {
                    m_CallBack(ekg->actualName(), ekg->processID(), ekg->threadID(), Abnormal_HeartbeatEvent,
                               ekg->length(), m_Verbose);
                }
                if (m_Verbose) {
                    std::cout << "Heartbeat " << ekg->actualName() << " - " << isAlive << "-" << isNormal << std::endl;
                }
            }
            catch (CorruptHeartbeat &ex) {
                if (m_Verbose) {
                    std::cout << "Heartbeat " << ekg->actualName() << " - corrupt" << std::endl;
                }
            }
            catch (const boost::interprocess::interprocess_exception &ex) {
                if (m_Verbose) {
                    std::cout << "Heartbeat " << ekg->actualName() << " not present - retrying" << std::endl;
                }
            }
        }
        boost::chrono::milliseconds scanPeriod = m_ScanPeriod;
        if (m_AutoScan) {
            scanPeriod = (smallestHB.count() / 2) ? boost::chrono::milliseconds(smallestHB.count() / 2)
                                                  : MIN_SCAN_PERIOD;
        }
        boost::this_thread::sleep_for(scanPeriod);
    }
}


void Watchdog::setCallback(boost::function<void(std::string &, pid_t, pid_t, HeartbeatEvent,
                                                boost::chrono::milliseconds, bool)> _f) {
    m_CallBack = _f;
}


void Watchdog::quiesce() {
    m_Running = false;
}


void Watchdog::scanHeartbeats() {
    if (!is_directory(SHARED_MEMORY_PATH)) {
        return;
    }
    // get contents of directory
    std::vector<directory_entry> dirList;
    copy(directory_iterator(SHARED_MEMORY_PATH), directory_iterator(), back_inserter(dirList));

    // extract set of heartbeats from directory
    boost::container::flat_set<std::string> candidateHBs;
    for (std::vector<directory_entry>::const_iterator it = dirList.begin(); it != dirList.end(); it++) {
        std::string path = (*it).path().string();
        size_t nameStart = path.find(HEARTBEATS_DIR) + HEARTBEATS_DIR.size();
        std::string actualName = path.substr(nameStart);
        if (Heartbeat::isHeartbeat(actualName)) {
            candidateHBs.insert(actualName);
        }
    }

    // compare candidates against our managed EKGs
    boost::container::flat_set<std::string> deadPIDs;
    for (boost::container::flat_set<std::string>::iterator it = candidateHBs.begin();
         it != candidateHBs.end(); it++) {
        std::string actualName = *it;
        pid_t processID = Heartbeat::extractProcessID(actualName);
        pid_t threadID = Heartbeat::extractThreadID(actualName);
        bool isAlive = (kill(processID, 0) == 0);
        if (isAlive) {
            if (m_Heartbeats.find(actualName) == m_Heartbeats.end()) {
                // this is a new heartbeat and the process behind it exists - start managing it
                boost::shared_ptr<EKG> ekg(new EKG(actualName));
                m_Heartbeats[actualName] = ekg;
                m_CallBack(actualName, processID, threadID, Started_HeartbeatEvent, boost::chrono::milliseconds(0),
                           m_Verbose);
            }
        } else {
            // apparently our process has died. Mark it for next of kin notification
            deadPIDs.insert(actualName);
        }
    }

    // refine down to a single proc name and pid for each dead heartbeat
    std::map<std::string, TProcInfo> deadProcs;
    for (boost::container::flat_set<std::string>::iterator it = deadPIDs.begin(); it != deadPIDs.end(); it++) {
        std::string actualName = *it;
        std::string procName = Heartbeat::extractProcName(actualName);
        TProcInfo procInfo;
        procInfo.m_ProcID = Heartbeat::extractProcessID(actualName);
        procInfo.m_ActualName = actualName;
        deadProcs[procName] = procInfo;
    }

    // do next of kin notification
    for (std::map<std::string, TProcInfo>::iterator it = deadProcs.begin(); it != deadProcs.end(); it++) {
        std::string procName = (*it).first;
        TProcInfo procInfo = (*it).second;
        if (m_Heartbeats.find(procInfo.m_ActualName) != m_Heartbeats.end()) {
            // we were already managing this heartbeat - notify next of kin and remove it from our list
            m_CallBack(procInfo.m_ActualName, procInfo.m_ProcID, 0, Fatal_HeartbeatEvent,
                       boost::chrono::milliseconds(0), m_Verbose);
            m_Heartbeats.erase(procInfo.m_ActualName);     // stop managing this heartbeat
        } else {
            // Not known to us.  Whatever it was, it's dead now - delete the remnant heartbeat
            boost::interprocess::shared_memory_object::remove(procInfo.m_ActualName.c_str());
        }
    }

    // stop managing dead heartbeats, and delete any unknown ones
    for (boost::container::flat_set<std::string>::iterator it = deadPIDs.begin(); it != deadPIDs.end(); it++) {
        std::string actualName = *it;
        if (m_Heartbeats.find(actualName) != m_Heartbeats.end()) {
            m_Heartbeats.erase(actualName);     // stop managing this heartbeat
        } else {
            // Not known to us.  Whatever it was, it's dead now - delete the remnant heartbeat
            boost::interprocess::shared_memory_object::remove(actualName.c_str());
        }
    }
}