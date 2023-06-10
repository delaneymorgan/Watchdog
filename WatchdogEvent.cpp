//
// Created by craig on 10/06/23.
//

#include "WatchdogEvent.h"


WatchdogEvent::WatchdogEvent(const std::string &actualName, pid_t processID, pid_t threadID,
                             HeartbeatEvent event, boost::chrono::milliseconds hbLength, int info) :
        m_ProcessName(Heartbeat::extractProcessName(actualName)),
        m_ThreadName(Heartbeat::extractThreadName(actualName)),
        m_ProcessID(processID),
        m_ThreadID(threadID),
        m_Event(event),
        m_HBLength(hbLength),
        m_Info(info) {
}

std::string WatchdogEvent::processName() const {
    return m_ProcessName;
}

std::string WatchdogEvent::threadName() const {
    return m_ThreadName;
}

pid_t WatchdogEvent::processID() const {
    return m_ProcessID;
}

pid_t WatchdogEvent::threadID() const {
    return m_ThreadID;
}

HeartbeatEvent WatchdogEvent::event() const {
    return m_Event;
}

boost::chrono::milliseconds WatchdogEvent::hbLength() const {
    return m_HBLength;
}

int WatchdogEvent::info() const {
    return m_Info;
}

