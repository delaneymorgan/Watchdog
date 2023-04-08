#include <iostream>
#include <csignal>
#include <boost/scoped_ptr.hpp>

#include "EKG.h"
#include "Watchdog.h"


boost::scoped_ptr<Watchdog> gWatchdog( new Watchdog(boost::chrono::milliseconds(1000)));


void signal_handler( int sig)
{
    if (sig == SIGINT)
    {
        gWatchdog->quiesce();
    }
}

void callBack(std::string& actualName, pid_t processID, pid_t threadID) {
    std::cout << "Heartbeat died: " << actualName << " - " << processID << "/" << threadID << std::endl;
}

int main() {
    signal( SIGINT, signal_handler);
    gWatchdog->setCallback(callBack);
    gWatchdog->monitor();
    std::cout << std::endl << "Finished!" << std::endl;
    return 0;
}
