//
// Created by craig on 6/04/23.
//

#include "EKG.h"

#include <stdexcept>

#include "Heartbeat.h"

#include <boost/interprocess/shared_memory_object.hpp>

using namespace boost::interprocess;


EKG::EKG(const std::string &heartbeatName) :
    m_Name(heartbeatName)
    {
    shared_memory_object shm;
    shm = shared_memory_object( open_only, heartbeatName.c_str(), boost::interprocess::read_only);
    m_Region = mapped_region( shm, boost::interprocess::read_only);
}

EKG::~EKG() {
    // don't release shared memory because another process could be using it
}

bool EKG::isAlive() {
    bool ret = false;
    Heartbeat beat;
    boost::chrono::system_clock::time_point timeNow = boost::chrono::system_clock::now();
    std::memcpy( reinterpret_cast<char*>(&beat), m_Region.get_address(), m_Region.get_size());
    if (!Heartbeat::isCRCOK( beat))
    {
        throw CorruptHeartbeat();
    }
    boost::chrono::milliseconds duration = boost::chrono::duration_cast<boost::chrono::milliseconds>(timeNow - beat.m_Beat);
    if (duration <= beat.m_Limit)
    {
        ret = true;
    }
    return ret;
}

std::string EKG::name() {
    return m_Name;
}

pid_t EKG::threadID() {
    Heartbeat beat;
    std::memcpy( reinterpret_cast<char*>(&beat), m_Region.get_address(), m_Region.get_size());
    if ( !Heartbeat::isCRCOK( beat))
    {
        throw CorruptHeartbeat();
    }
    return beat.m_ThreadID;
}