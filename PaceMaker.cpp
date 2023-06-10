/**
 * @file PaceMaker.cpp
 *
 * @brief the PaceMaker class is used by a Watchdog client to manage the heartbeat for a single thread.
 *
 * @copyright Delaney & Morgan Computing
 */

#include "PaceMaker.h"

#include <stdexcept>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/chrono/duration.hpp>

#include "Heartbeat.h"


using namespace boost::interprocess;


PaceMaker::PaceMaker(const std::string &processName, const std::string &threadName,
                     boost::chrono::milliseconds normalLimit,
                     boost::chrono::milliseconds absoluteLimit) :
        m_ProcessName(processName),
        m_ThreadName(threadName),
        m_ActualName(Heartbeat::makeActualName(processName, threadName)),
        m_NormalLimit(normalLimit),
        m_AbsoluteLimit(absoluteLimit) {
    if (!Heartbeat::isHeartbeat(m_ActualName)) {
        throw InvalidHeartbeatName();
    }
    shared_memory_object shm;
    shm = shared_memory_object(open_or_create, m_ActualName.c_str(), read_write);
    Heartbeat beat;
    beat.m_NormalLimit = m_NormalLimit;
    beat.m_AbsoluteLimit = m_AbsoluteLimit;
    shm.truncate(sizeof(beat));
    m_Region = mapped_region(shm, read_write);
    std::memcpy(m_Region.get_address(), reinterpret_cast<char *>(&beat), m_Region.get_size());
}


PaceMaker::~PaceMaker() {
    // don't release shared memory - that's the watchdog's job
}

/**
 * pulse the heartbeat
 */
void PaceMaker::pulse(int info) {
    boost::mutex::scoped_lock beatMutex(m_BeatMutex);
    Heartbeat beat;
    beat.m_NormalLimit = m_NormalLimit;
    beat.m_AbsoluteLimit = m_AbsoluteLimit;
    beat.m_Beat = Heartbeat::tickCountNow();
    beat.m_Info = info;
    Heartbeat::SetCRC(beat);
    std::memcpy(m_Region.get_address(), reinterpret_cast<char *>( &beat), m_Region.get_size());
}


