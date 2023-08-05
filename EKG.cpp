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


EKG::EKG(const std::string &actualName) :
        m_ActualName(actualName),
        m_ProcessID(Heartbeat::extractProcessID(actualName)),
        m_ThreadID(Heartbeat::extractThreadID(actualName)),
        m_SharedMemory(m_ActualName) {
}


EKG::~EKG() {
    // don't release shared memory because another process could be using it
}


bool EKG::isAlive() {
    typedef boost::chrono::milliseconds TMSec;
    bool ret = false;
    Heartbeat beat;
    TMSec duration = length();
    m_SharedMemory.read(&beat, sizeof(beat));
    if (duration <= beat.m_AbsoluteLimit) {
        ret = true;
    }
    return ret;
}


bool EKG::isNormal() {
    typedef boost::chrono::milliseconds TMSec;
    bool ret = false;
    TMSec duration = length();
    Heartbeat beat;
    m_SharedMemory.read(&beat, sizeof(beat));
    if (duration <= beat.m_NormalLimit) {
        ret = true;
    }
    return ret;
}

int EKG::info() {
    Heartbeat beat;
    m_SharedMemory.read(&beat, sizeof(beat));
    return beat.m_Info;
}


std::string EKG::actualName() {
    return m_ActualName;
}


boost::chrono::milliseconds EKG::normalLimit() {
    Heartbeat beat;
    m_SharedMemory.read(&beat, sizeof(beat));
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
    TTickCount timeNow = Heartbeat::tickCountNow();
    m_SharedMemory.read(&beat, sizeof(beat));
    if (!Heartbeat::isCRCOK(beat)) {
        throw CorruptHeartbeat();
    }
    TMSec hbLength;
    if (timeNow >= beat.m_Beat) {
        hbLength = TMSec(timeNow) - TMSec(beat.m_Beat);
    } else {
        // tick count has wrapped around zero
        hbLength = TMSec(timeNow) - TMSec(static_cast<int64_t>(beat.m_Beat));
    }
    return hbLength;
}
