/**
 * @file PaceMaker.cpp
 *
 * @brief the PaceMaker class is used by a Watchdog client to manage the heartbeat for a single thread.
 *
 * @copyright Delaney & Morgan Computing
 */

#include "PaceMaker.h"

#include <stdexcept>

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
        m_AbsoluteLimit(absoluteLimit),
        m_SharedMemory(m_ActualName, sizeof(Heartbeat)) {
    if (!Heartbeat::isHeartbeat(m_ActualName)) {
        throw InvalidHeartbeatName();
    }
    pulse();
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
    m_SharedMemory.write(&beat, sizeof(beat));
}


