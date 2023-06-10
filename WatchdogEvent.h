//
// Created by craig on 10/06/23.
//

#ifndef WATCHDOG_WATCHDOGEVENT_H
#define WATCHDOG_WATCHDOGEVENT_H

#include <string>
#include <boost/chrono.hpp>

#include "Heartbeat.h"
#include "EKG.h"

class WatchdogEvent {
public:
    WatchdogEvent(const std::string &actualName, pid_t processID, pid_t threadID, HeartbeatEvent event,
                  boost::chrono::milliseconds hbLength, int info = 0);
    virtual ~WatchdogEvent() {};

    std::string processName() const;
    std::string threadName() const;
    pid_t processID() const;
    pid_t threadID() const;
    HeartbeatEvent event() const;
    boost::chrono::milliseconds hbLength() const;
    int info() const;

private:
    std::string m_ProcessName;
    std::string m_ThreadName;
    pid_t m_ProcessID;
    pid_t m_ThreadID;
    HeartbeatEvent m_Event;
    boost::chrono::milliseconds m_HBLength;
    int m_Info;
};


#endif //WATCHDOG_WATCHDOGEVENT_H
