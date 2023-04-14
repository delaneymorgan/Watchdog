//
// Created by craig on 6/04/23.
//

#include "Watchdog.h"

#include <iostream>
#include <csignal>

#include <boost/filesystem.hpp>
#include <boost/thread/thread.hpp>

#include "Heartbeat.h"


using namespace boost::filesystem;

namespace {
    const std::string HEARTBEATS_DIR = "/dev/shm/";
    const std::string SHARED_MEMORY_PATH = "/dev/shm";
    typedef std::map<std::string, boost::shared_ptr<EKG> >::iterator THeartbeatIterator;
}


Watchdog::Watchdog( boost::chrono::milliseconds scanPeriod, bool autoScan, bool verbose ) :
        m_Running( true ),
        m_AutoScan( autoScan ),
        m_Verbose( verbose ),
        m_ScanPeriod( scanPeriod ) {
}


Watchdog::~Watchdog() {
}


void Watchdog::monitor() {
    while ( m_Running ) {
        scanHeartbeats();
        for ( THeartbeatIterator it = m_Heartbeats.begin(); it != m_Heartbeats.end(); it++ ) {
            boost::shared_ptr<EKG> ekg = it->second;
            try {
                std::string isAlive = ekg->isAlive() ? "alive" : "dead";
                std::string isNormal = ekg->isNormal() ? "normal" : "warning";
                if ( !ekg->isAlive()) {
                    m_CallBack( ekg->actualName(), ekg->processID(), ekg->threadID(), Fatal_HeartbeatState,
                                ekg->length());
                } else if ( !ekg->isAlive()) {
                    m_CallBack( ekg->actualName(), ekg->processID(), ekg->threadID(), Abnormal_HeartbeatState,
                                ekg->length());
                }
                if ( m_Verbose ) {
                    std::cout << "Heartbeat " << ekg->actualName() << " - " << isAlive << "-" << isNormal << std::endl;
                }
            }
            catch ( CorruptHeartbeat &ex ) {
                if ( m_Verbose ) {
                    std::cout << "Heartbeat " << ekg->actualName() << " - corrupt" << std::endl;
                }
            }
            catch ( const boost::interprocess::interprocess_exception &ex ) {
                if ( m_Verbose ) {
                    std::cout << "Heartbeat " << ekg->actualName() << " not present - retrying" << std::endl;
                }
            }
        }
        boost::this_thread::sleep_for( m_ScanPeriod );
    }
}


void Watchdog::setCallback( boost::function<void( std::string &, pid_t, pid_t, HeartbeatState,
                                                  boost::chrono::milliseconds )> _f ) {
    m_CallBack = _f;
}


void Watchdog::quiesce() {
    m_Running = false;
}


void Watchdog::scanHeartbeats() {
    if ( !is_directory( SHARED_MEMORY_PATH )) {
        return;
    }
    // get contents of directory
    std::vector<directory_entry> dirList;
    copy( directory_iterator( SHARED_MEMORY_PATH ), directory_iterator(), back_inserter( dirList ));

    // extract set of heartbeats from directory
    boost::container::flat_set<std::string> candidateHBs;
    for ( std::vector<directory_entry>::const_iterator it = dirList.begin(); it != dirList.end(); it++ ) {
        std::string path = (*it).path().string();
        size_t nameStart = path.find( HEARTBEATS_DIR ) + HEARTBEATS_DIR.size();
        std::string actualName = path.substr( nameStart );
        if ( Heartbeat::isHeartbeat( actualName )) {
            candidateHBs.insert( actualName );
        }
    }

    // compare candidates against our managed EKGs
    std::vector<std::string> hitList;
    for ( boost::container::flat_set<std::string>::iterator it = candidateHBs.begin();
          it != candidateHBs.end(); it++ ) {
        std::string actualName = *it;
        pid_t processID = Heartbeat::extractProcessID( actualName );
        bool isAlive = (kill( processID, 0 ) == 0);
        if ( m_Heartbeats.find( actualName ) != m_Heartbeats.end()) {
            // we're already managing this heartbeat
            if ( !isAlive ) {
                // apparently our process has died. Notify next of kin
                pid_t threadID = Heartbeat::extractThreadID( actualName );
                m_CallBack( actualName, processID, threadID, Fatal_HeartbeatState, boost::chrono::milliseconds( 0 ));
                hitList.push_back( actualName );
            }
        } else if ( isAlive ) {
            // this is a new heartbeat and the process behind it exists - start managing it
            boost::shared_ptr<EKG> ekg( new EKG( actualName ));
            m_Heartbeats[actualName] = ekg;
        } else {
            // Whatever it was, it's dead now - delete it
            std::string path = HEARTBEATS_DIR + actualName;
            boost::filesystem::remove( path );
        }
    }

    // check for superfluous heartbeat management
    for ( THeartbeatIterator it = m_Heartbeats.begin(); it != m_Heartbeats.end(); it++ ) {
        if ( !candidateHBs.contains( it->first )) {
            // heartbeat no longer exists
            // TODO: not sure how this ever happens
            hitList.push_back( it->first );
        }
    }

    // take out any obsolete heartbeats
    for ( std::vector<std::string>::iterator it = hitList.begin(); it != hitList.end(); it++ ) {
        m_Heartbeats.erase( *it );
    }
}