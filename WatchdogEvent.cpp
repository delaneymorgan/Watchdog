//
// Created by craig on 10/06/23.
//

#include "WatchdogEvent.h"

/*
WatchdogEvent::WatchdogEvent(const std::string& procName, const std::string& threadName, pid_t procID, pid_t threadID,
                             HeartbeatEvent event,boost::chrono::milliseconds hbLength, int info) :
                             m_ProcName(procName),
                             m_ThreadName(threadName),
                             m_ProcID(procID),
                             m_ThreadID(threadID),
                             m_Event(event),
                             m_HBLength(hbLength),
                             m_Info(info) {
}
*/

WatchdogEvent::WatchdogEvent(const std::string& actualName, pid_t procID, pid_t threadID,
                             HeartbeatEvent event,boost::chrono::milliseconds hbLength, int info) :
        m_ProcName(Heartbeat::extractProcName(actualName)),
        m_ThreadName(Heartbeat::extractThreadName(actualName)),
        m_ProcID(procID),
        m_ThreadID(threadID),
        m_Event(event),
        m_HBLength(hbLength),
        m_Info(info) {
}

std::string WatchdogEvent::procName() const {
    return m_ProcName;
}

std::string WatchdogEvent::threadName() const {
    return m_ThreadName;
}

pid_t WatchdogEvent::procID() const {
    return m_ProcID;
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

