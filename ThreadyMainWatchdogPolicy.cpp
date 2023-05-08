/**
 * @file ThreadyMainWatchdogPolicy.cpp
 *
 * @brief a specific watchdog policy for the ThreadyMain application
 *
 * @copyright Delaney & Morgan Computing
 */

#include "ThreadyMainWatchdogPolicy.h"
#include "macros.h"

#include <iostream>


ThreadyMainWatchdogPolicy::ThreadyMainWatchdogPolicy() :
        IWatchdogPolicy("ThreadyMain") {
}

ThreadyMainWatchdogPolicy::~ThreadyMainWatchdogPolicy() {

}

/**
 * a policy-specifc callback for handling Watchdog events, just for this application
 *
 * @param actualName the name of the heartbeat in shared memory
 * @param processID the heartbeat's process id
 * @param threadID the heartbeat's thread id (0 => no thread)
 * @param event the heartbeat event type
 * @param hbLength the duration since the heartbeat's last pulse
 * @param verbose true => versbose, false => otherwise
 */
void ThreadyMainWatchdogPolicy::handleEvent(std::basic_string<char> actualName, pid_t processID, pid_t threadID,
                                            HeartbeatEvent event, boost::chrono::milliseconds hbLength, bool verbose) {

    std::string eventName = Heartbeat::heartbeatEventName(event);
    switch (event) {
        case Started_HeartbeatEvent:
            if (verbose) {
                std::cout << QUOTE(ThreadyMainWatchdogPolicy) << ": Heartbeat started: " <<
                    Heartbeat::extractProcName(actualName) << ":" <<
                    Heartbeat::extractThreadName(actualName) << " - " << processID << "/" <<
                    threadID << std::endl;
            }
            break;

        case Slow_HeartbeatEvent:
            if (verbose) {
                std::cout << QUOTE(ThreadyMainWatchdogPolicy) << ": Heartbeat slow: " <<
                    Heartbeat::extractProcName(actualName) << ":" <<
                    Heartbeat::extractThreadName(actualName) << " - " << processID << "/" <<
                    threadID << " = " << hbLength.count() << " mSec" << std::endl;
            }
            // here we can choose to vary process' priority with rnice
            // or kill it and make it restart
            break;

        case Hung_HeartbeatEvent:
            if (verbose) {
                std::cout << QUOTE(ThreadyMainWatchdogPolicy) <<
                    ": Process hung: " << Heartbeat::extractProcName(actualName) << " - " << processID << std::endl;
            }
            break;

        case Dead_HeartbeatEvent:
            if (verbose) {
                std::cout << QUOTE(ThreadyMainWatchdogPolicy) <<
                    ": Process died: " << Heartbeat::extractProcName(actualName) << " - " << processID << std::endl;
            }
            break;

        default:
            break;
    }
}
