/**
 * @file ThreadyMainWatchdogPolicy.cpp
 *
 * @brief a specific watchdog policy for the ThreadyMain application
 *
 * @copyright Delaney & Morgan Computing
 */

#include "ThreadyMainWatchdogPolicy.h"

#include "ThreadyState.h"
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
 * @param event the Watchdog event
 * @param verbose true => versbose, false => otherwise
 */
void ThreadyMainWatchdogPolicy::handleEvent(const WatchdogEvent &event, bool verbose) {

    std::string eventName = Heartbeat::heartbeatEventName(event.event());
    switch (event.event()) {
        case Started_HeartbeatEvent:
            if (verbose) {
                std::cout << QUOTE(ThreadyMainWatchdogPolicy) << ": Heartbeat started: " <<
                          event.procName() << ":" <<
                          event.threadName() << " - " << event.procID() << "/" <<
                          event.threadID() << std::endl;
            }
            break;

        case Slow_HeartbeatEvent:
            if (verbose) {
                std::cout << QUOTE(ThreadyMainWatchdogPolicy) << ": Heartbeat slow: " <<
                          event.procName() << ":" <<
                          event.threadName() << " - " << event.procID() << "/" <<
                          event.threadID() << " (" << threadyStateName(ThreadyState(event.info())) << ") = " <<
                          event.hbLength().count() << " mSec" << std::endl;
            }
            // here we can choose to vary process' priority with rnice
            // or kill it and make it restart
            break;

        case Hung_HeartbeatEvent:
            if (verbose) {
                std::cout << QUOTE(ThreadyMainWatchdogPolicy) << ": Heartbeat hung: " <<
                          event.procName() << ":" <<
                          event.threadName() << " - " << event.procID() << "/" <<
                          event.threadID() << " (" << threadyStateName(ThreadyState(event.info())) << ") = " <<
                          event.hbLength().count() << " mSec" << std::endl;
            }
            break;

        case Dead_HeartbeatEvent:
            if (verbose) {
                std::cout << QUOTE(ThreadyMainWatchdogPolicy) <<
                          ": Process died: " << event.procName() << " - " << event.procID() << std::endl;
            }
            break;

        default:
            break;
    }
}
