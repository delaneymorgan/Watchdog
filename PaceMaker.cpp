//
// Created by craig on 6/04/23.
//

#include "PaceMaker.h"

#include <stdexcept>
#include <sys/syscall.h>

#include <boost/interprocess/shared_memory_object.hpp>

#include "Heartbeat.h"


using namespace boost::interprocess;


PaceMaker::PaceMaker(const std::string &heartbeatName, boost::chrono::milliseconds heartbeatLimit) :
m_Name(heartbeatName),
m_Limit(heartbeatLimit)
{
    shared_memory_object shm;
    shm = shared_memory_object( open_or_create, heartbeatName.c_str(), read_write);
    Heartbeat beat;
    beat.m_Limit = m_Limit;
    shm.truncate( sizeof( beat));
    m_Region = mapped_region( shm, read_write);
    std::memcpy( m_Region.get_address(), reinterpret_cast<char*>(&beat), m_Region.get_size());
}

PaceMaker::~PaceMaker() {
    // don't release shared memory because another process could be using it
    // TODO: OR NOT?
}

void PaceMaker::beat() {
    boost::mutex::scoped_lock beatMutex(m_BeatMutex);
    Heartbeat beat;
    beat.m_Limit = m_Limit;
    beat.m_Beat = boost::chrono::system_clock::now();
    beat.m_ThreadID = syscall( __NR_gettid);
    Heartbeat::SetCRC(beat);
    std::memcpy( m_Region.get_address(), reinterpret_cast<char*>( &beat), m_Region.get_size());
}

std::string PaceMaker::name() {
    return m_Name;
}