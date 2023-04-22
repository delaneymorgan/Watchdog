//
// Created by craig on 22/04/23.
//

#include "ThreadyWatchdogPolicy.h"

#include <iostream>


ThreadyWatchdogPolicy::ThreadyWatchdogPolicy() :
        IWatchdogPolicy("ThreadyMain") {
}

ThreadyWatchdogPolicy::~ThreadyWatchdogPolicy() {

}

void ThreadyWatchdogPolicy::handleEvent(std::basic_string<char> actualName, pid_t processID, pid_t threadID,
                                        HeartbeatEvent event, boost::chrono::milliseconds hbLength, bool verbose) {

    std::string eventName = Heartbeat::heartbeatEventName(event);
    switch (event) {
        case Started_HeartbeatEvent:
            if (verbose) {
                std::cout << "ThreadyWatchdogPolicy: Heartbeat started: "
                          << Heartbeat::extractProcName(actualName) <<
                          ":" << Heartbeat::extractThreadName(actualName) << " - " << processID << "/" <<
                          threadID << std::endl;
            }
            break;

        case Slow_HeartbeatEvent:
            if (verbose) {
                std::cout << "ThreadyWatchdogPolicy: Heartbeat slow: "
                          << Heartbeat::extractProcName(actualName) <<
                          ":" << Heartbeat::extractThreadName(actualName) << " - " << processID << "/" <<
                          threadID << " = " << hbLength.count() << " mSec" << std::endl;
            }
            // here we can choose to vary process' priority with rnice
            // or kill it and make it restart
            break;

        case Hung_HeartbeatEvent:
            if (verbose) {
                std::cout << "ThreadyWatchdogPolicy: Process hung: " << Heartbeat::extractProcName(actualName) <<
                          " - " << processID << std::endl;
            }
            break;

        case Dead_HeartbeatEvent:
            if (verbose) {
                std::cout << "ThreadyWatchdogPolicy: Process died: " << Heartbeat::extractProcName(actualName) <<
                          " - " << processID << std::endl;
            }
            break;

        default:
            break;
    }
}
