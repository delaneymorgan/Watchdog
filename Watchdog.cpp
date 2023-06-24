/**
 * @file Watchdog.cpp
 *
 * @brief the Watchdog class used by a watchdog application to manage heartbeats
 *
 * @copyright Delaney & Morgan Computing
 */

#include "Watchdog.h"

#include "SharedMemory.h"

#include <iostream>
#include <csignal>

#include <boost/thread/thread.hpp>
#include <boost/interprocess/shared_memory_object.hpp>


#include "Heartbeat.h"


using namespace boost::filesystem;

namespace {
    const boost::chrono::milliseconds MIN_SCAN_PERIOD(10);
    const std::string HEARTBEATS_DIR = "/dev/shm/";
    const std::string SHARED_MEMORY_PATH = "/dev/shm";
    typedef std::map<std::string, boost::shared_ptr<EKG> >::iterator THeartbeatIterator;
}


Watchdog::Watchdog(boost::chrono::milliseconds scanPeriod, bool autoScan, bool verbose) :
        m_Running(true),
        m_AutoScan(autoScan),
        m_Verbose(verbose),
        m_ScanPeriod(scanPeriod) {
}


Watchdog::~Watchdog() {
}

/**
 * perform callbacks, both the global callback and any specific policy callback
 *
 * @param event the watchdog event
 */
void Watchdog::doCallbacks(const WatchdogEvent &event) {
    if (!m_CallBack.empty())
    {
        m_CallBack(event, m_Verbose);
    }
    std::map<std::string, boost::shared_ptr<IWatchdogPolicy> >::iterator policy = m_Policies.find(event.processName());
    if (policy != m_Policies.end()) {
        (*policy).second->handleEvent(event, m_Verbose);
    }
}

/**
 * perform callbacks for ekgs
 *
 * @param ekg the ekg with an event to report
 * @param event the event type
 */
void Watchdog::doCallbacks(const boost::shared_ptr<EKG> &ekg, HeartbeatEvent event) {
    WatchdogEvent wdevent(ekg->actualName(), ekg->processID(), ekg->threadID(), event,
                          ekg->length(), ekg->info());
    doCallbacks(wdevent);
}

/**
 * Watchdog's main loop
 */
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
                std::string processName = Heartbeat::extractProcessName(ekg->actualName());
                std::string isAlive = ekg->isAlive() ? "alive" : "dead";
                std::string isNormal = ekg->isNormal() ? "normal" : "warning";
                if (!ekg->isAlive()) {
                    doCallbacks(ekg, Hung_HeartbeatEvent);
                } else if (!ekg->isNormal()) {
                    doCallbacks(ekg, Slow_HeartbeatEvent);
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

/**
 * set the global callback for the watchdog
 * @param _f the callback function
 */
void Watchdog::setCallback(const boost::function<void(WatchdogEvent, bool)> &_f) {
    m_CallBack = _f;
}

/**
 * add a policy for managing hearbeat events
 *
 * @param policy the policy for a specific application
 */
void Watchdog::setPolicy(const boost::shared_ptr<IWatchdogPolicy> &policy) {
    m_Policies[policy->processName()] = policy;
}

/**
 * quietly let go and die
 */
void Watchdog::quiesce() {
    m_Running = false;
}

/**
 * scan shared memory for heartbeats, checking for events and notifying callbacks and policies
 *
 */
void Watchdog::scanHeartbeats() {
    if (!is_directory(SHARED_MEMORY_PATH)) {
        return;
    }
    // get contents of directory
    std::vector<directory_entry> dirList;
    copy(directory_iterator(SHARED_MEMORY_PATH), directory_iterator(), back_inserter(dirList));

    // extract set of heartbeats from directory
    boost::container::flat_set<std::string> candidateHBs = extractCandidateHBs(dirList);

    // compare candidates against our managed EKGs
    boost::container::flat_set<std::string> deadPIDs = compareAgainstEKGs(candidateHBs);

    // refine down to a single proc processName and pid for each dead heartbeat
    std::map<std::string, TProcInfo> deadProcs = getDeadProcs(deadPIDs);

    // do next of kin notification
    notifyStakeholders(deadProcs);

    // stop managing dead heartbeats, and delete any unknown ones
    cleanHeartbeats(deadPIDs);
}

/**
 * extracts a set of heartbeat names from the contents of shared memory
 *
 * @param dirList the contents of shared memory
 * @return the set of heartbeat names
 */
boost::container::flat_set<std::string> Watchdog::extractCandidateHBs(const std::vector<directory_entry> &dirList) {
    boost::container::flat_set<std::string> candidateHBs;
    for (std::vector<directory_entry>::const_iterator it = dirList.begin(); it != dirList.end(); it++) {
        std::string path = (*it).path().string();
        size_t nameStart = path.find(HEARTBEATS_DIR) + HEARTBEATS_DIR.size();
        std::string actualName = path.substr(nameStart);
        if (Heartbeat::isHeartbeat(actualName)) {
            candidateHBs.insert(actualName);
        }
    }
    return candidateHBs;
}

/**
 * compare candidate heartbeats against our EKGs to see if any stakeholders need notification of application starts
 * also return the heartbeat names not belonging to any live processes
 *
 * @param candidateHBs all the heartbeats in shared memory
 * @return the set of heartbeat names belonging to dead processes
 */
boost::container::flat_set<std::string>
Watchdog::compareAgainstEKGs(boost::container::flat_set<std::string> &candidateHBs) {
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
                WatchdogEvent wdEvent(actualName, processID, threadID, Started_HeartbeatEvent,
                                      boost::chrono::milliseconds(0));
                doCallbacks(wdEvent);
            }
        } else {
            // apparently our process has died. Mark it for next of kin notification
            deadPIDs.insert(actualName);
        }
    }
    return deadPIDs;
}

/**
 * get the application information for each dead heartbeat
 *
 * @param deadPIDs the set of heartbeats that no longer have processes
 * @return a map of process names and their info
 */
std::map<std::string, TProcInfo> Watchdog::getDeadProcs(boost::container::flat_set<std::string> &deadPIDs) {
    std::map<std::string, TProcInfo> deadProcs;
    for (boost::container::flat_set<std::string>::iterator it = deadPIDs.begin(); it != deadPIDs.end(); it++) {
        std::string actualName = *it;
        std::string processName = Heartbeat::extractProcessName(actualName);
        TProcInfo procInfo;
        procInfo.m_ProcessID = Heartbeat::extractProcessID(actualName);
        procInfo.m_ActualName = actualName;
        deadProcs[processName] = procInfo;
    }
    return deadProcs;
}

/**
 * notify stakeholders that the application (and all its threads) have died
 *
 * @param deadProcs the map of dead heartbeats and their processes
 */
void Watchdog::notifyStakeholders(std::map<std::string, TProcInfo> &deadProcs) {
    for (std::map<std::string, TProcInfo>::iterator it = deadProcs.begin(); it != deadProcs.end(); it++) {
        std::string processName = (*it).first;
        TProcInfo procInfo = (*it).second;
        if (m_Heartbeats.find(procInfo.m_ActualName) != m_Heartbeats.end()) {
            // we were already managing this heartbeat - notify next of kin and remove it from our list
            WatchdogEvent wdEvent(procInfo.m_ActualName, procInfo.m_ProcessID, 0, Dead_HeartbeatEvent,
                                  boost::chrono::milliseconds(0));
            doCallbacks(wdEvent);
            m_Heartbeats.erase(procInfo.m_ActualName);     // stop managing this heartbeat
        } else {
            // Not known to us.  Whatever it was, it's dead now - delete the remnant heartbeat
            boost::interprocess::shared_memory_object::remove(procInfo.m_ActualName.c_str());
        }
    }
}

/**
 * clean up obsolete heartbeats from shared memory, both ones we were managing,
 * and any others belonging to dead processes
 *
 * @param deadPIDs the set of dead process ids
 */
void Watchdog::cleanHeartbeats(boost::container::flat_set<std::string> &deadPIDs) {
    for (boost::container::flat_set<std::string>::iterator it = deadPIDs.begin(); it != deadPIDs.end(); it++) {
        std::string actualName = *it;
        if (m_Heartbeats.find(actualName) != m_Heartbeats.end()) {
            m_Heartbeats.erase(actualName);     // stop managing this heartbeat
        } else {
            // Not known to us.  Whatever it was, it's dead now - delete the remnant heartbeat
            SharedMemory::remove(actualName);
        }
    }
}