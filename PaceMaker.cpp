//
// Created by craig on 6/04/23.
//

#include "PaceMaker.h"

#include <stdexcept>
#include <sys/syscall.h>

#include <boost/interprocess/shared_memory_object.hpp>

#include "Heartbeat.h"


using namespace boost::interprocess;


PaceMaker::PaceMaker(const std::string &userName, boost::chrono::milliseconds normalLimit, boost::chrono::milliseconds absoluteLimit) :
        m_UserName(userName),
        m_ActualName(Heartbeat::makeActualName(userName)),
        m_NormalLimit(normalLimit),
        m_AbsoluteLimit(absoluteLimit),
        m_ProcessID(getpid()),
        m_ThreadID(gettid()) {
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
    // don't release shared memory because another process could be using it
    // TODO: OR NOT?
}

void PaceMaker::beat() {
    boost::mutex::scoped_lock beatMutex(m_BeatMutex);
    Heartbeat beat;
    beat.m_NormalLimit = m_NormalLimit;
    beat.m_AbsoluteLimit = m_AbsoluteLimit;
    beat.m_Beat = boost::chrono::system_clock::now();
    Heartbeat::SetCRC(beat);
    std::memcpy(m_Region.get_address(), reinterpret_cast<char *>( &beat), m_Region.get_size());
}

std::string PaceMaker::userName() {
    return m_UserName;
}

std::string PaceMaker::actualName() {
    return m_ActualName;
}

pid_t PaceMaker::processID() {
    return m_ProcessID;
}

pid_t PaceMaker::threadID() {
    return m_ThreadID;
}