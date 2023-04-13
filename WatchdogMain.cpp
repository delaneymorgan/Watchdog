#include <iostream>
#include <csignal>
#include <boost/scoped_ptr.hpp>

#include "EKG.h"
#include "Watchdog.h"


boost::scoped_ptr<Watchdog> gWatchdog( new Watchdog(boost::chrono::milliseconds(1000), true));


void signal_handler( int sig)
{
    if (sig == SIGINT)
    {
        gWatchdog->quiesce();
    }
}

void callBack(std::string& actualName, pid_t processID, pid_t threadID, HeartbeatState state) {
    switch (state) {
        case Abnormal_HeartbeatState:
            std::cout << "Heartbeat abnormal: " << actualName << " - " << processID << "/" << threadID << std::endl;
            break;
        case Fatal_HeartbeatState:
            std::cout << "Heartbeat fatal: " << actualName << " - " << processID << "/" << threadID << std::endl;
            break;
        default:
            break;
    }
}

int main() {
    signal( SIGINT, signal_handler);
    gWatchdog->setCallback(callBack);
    gWatchdog->monitor();
    std::cout << std::endl << "Finished!" << std::endl;
    return 0;
}
