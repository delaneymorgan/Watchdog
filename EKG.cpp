/**
 * @file EKG.cpp
 *
 * @brief the EKG class used by the Watchdog class to manage a single heartbeat
 *
 * @copyright Delaney & Morgan Computing
 */

#include "EKG.h"

#include <stdexcept>

#include "Heartbeat.h"

#include <boost/interprocess/shared_memory_object.hpp>


using namespace boost::interprocess;


EKG::EKG( const std::string &actualName ) :
        m_ActualName( actualName ),
        m_ProcessID( getpid()),
        m_ThreadID( gettid()) {
    shared_memory_object shm;
    shm = shared_memory_object( open_only, m_ActualName.c_str(), boost::interprocess::read_only );
    m_Region = mapped_region( shm, boost::interprocess::read_only );
}


EKG::~EKG() {
    // don't release shared memory because another process could be using it
}


bool EKG::isAlive() {
    bool ret = false;
    Heartbeat beat;
    boost::chrono::system_clock::time_point timeNow = boost::chrono::system_clock::now();
    std::memcpy( reinterpret_cast<char *>(&beat), m_Region.get_address(), m_Region.get_size());
    if ( !Heartbeat::isCRCOK( beat )) {
        throw CorruptHeartbeat();
    }
    boost::chrono::milliseconds duration = boost::chrono::duration_cast<boost::chrono::milliseconds>(
            timeNow - beat.m_Beat );
    if ( duration <= beat.m_AbsoluteLimit ) {
        ret = true;
    }
    return ret;
}


bool EKG::isNormal() {
    typedef boost::chrono::milliseconds TMSec;
    bool ret = false;
    Heartbeat beat;
    boost::chrono::system_clock::time_point timeNow = boost::chrono::system_clock::now();
    std::memcpy( reinterpret_cast<char *>(&beat), m_Region.get_address(), m_Region.get_size());
    if ( !Heartbeat::isCRCOK( beat )) {
        throw CorruptHeartbeat();
    }
    TMSec duration = boost::chrono::duration_cast<TMSec>(timeNow - beat.m_Beat );
    if ( duration <= beat.m_NormalLimit ) {
        ret = true;
    }
    return ret;
}


std::string EKG::actualName() {
    return m_ActualName;
}


boost::chrono::milliseconds EKG::normalLimit() {
    Heartbeat beat;
    std::memcpy( reinterpret_cast<char *>(&beat), m_Region.get_address(), m_Region.get_size());
    return beat.m_NormalLimit;
}


pid_t EKG::processID() const {
    return m_ProcessID;
}


pid_t EKG::threadID() const {
    return m_ThreadID;
}


boost::chrono::milliseconds EKG::length() {
    typedef boost::chrono::milliseconds TMSec;
    Heartbeat beat;
    boost::chrono::system_clock::time_point timeNow = boost::chrono::system_clock::now();
    std::memcpy( reinterpret_cast<char *>(&beat), m_Region.get_address(), m_Region.get_size());
    if ( !Heartbeat::isCRCOK( beat )) {
        throw CorruptHeartbeat();
    }
    TMSec hbLength = boost::chrono::duration_cast<TMSec>(timeNow - beat.m_Beat );
    return hbLength;
}
