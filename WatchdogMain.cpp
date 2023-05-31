/**
 * @file WatchdogMain.cpp
 *
 * @brief this is an example program demonstrating the structure of a Watchdog application.
 * It creates an instance of the Watchdog class, which in turn will create as many EKG instances as required to
 * manage heartbeats appearing in shared memory.
 *
 * @copyright Delaney & Morgan Computing
 */

#include <iostream>
#include <csignal>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/program_options.hpp>

#include "Watchdog.h"
#include "ThreadyMainWatchdogPolicy.h"


boost::shared_ptr<Watchdog> gWatchdog;


void signal_handler(int sig) {
    if (sig == SIGINT) {
        gWatchdog->quiesce();
    }
}


/**
 * a global callback for Watchdog - all events will be reported here,
 * as well as the policy-specific callbacks.
 *
 * @param actualName the full processName of the heartbeat
 * @param processID the process id of the monitored process
 * @param threadID the thread id within the monitored process
 * @param event - event description
 * @param hbLength the length of the heartbeat that triggered the event
 * @param info optional client state during event
 * @param verbose true => verbose, false => quiet
 */
void callBack(std::string &actualName, pid_t processID, pid_t threadID, HeartbeatEvent event,
              boost::chrono::milliseconds hbLength, int info, bool verbose) {
    std::string eventName = Heartbeat::heartbeatEventName(event);
    if (verbose) {
        switch (event) {
            case Started_HeartbeatEvent:
                std::cout << "Heartbeat started: " << Heartbeat::extractProcName(actualName) <<
                          ":" << Heartbeat::extractThreadName(actualName) << " - " << processID << "/" <<
                          threadID << std::endl;
                break;

            case Slow_HeartbeatEvent:
                std::cout << "Heartbeat slow: " << Heartbeat::extractProcName(actualName) <<
                          ":" << Heartbeat::extractThreadName(actualName) << " - " << processID << "/" <<
                          threadID << " (" << info << ") = " << hbLength.count() << " mSec" << std::endl;
                break;

            case Hung_HeartbeatEvent:
                std::cout << "Heartbeat hung: " << Heartbeat::extractProcName(actualName) <<
                          ":" << Heartbeat::extractThreadName(actualName) << " - " << processID << "/" <<
                          threadID << " (" << info << ") = " << hbLength.count() << " mSec" << std::endl;
                break;

            case Dead_HeartbeatEvent:
                std::cout << "Process died: " << Heartbeat::extractProcName(actualName) <<
                          " - " << processID << std::endl;
                break;

            default:
                break;
        }
    }
}


namespace po = boost::program_options;
namespace postyle = boost::program_options::command_line_style;


po::variables_map Usage(int argc, char *argv[]) {
    po::options_description desc("Options");
    desc.add_options()
            ("auto,a", "Auto-scale scan rate")
            ("verbose,v", "Verbose mode")
            ("help,?", "Print this help message");
    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
        if (vm.count("help")) {
            std::cout << "Usage: " << argv[0] << std::endl << desc << std::endl;
            exit(0);
        }
    }
    catch (po::error &ex) {
        std::cerr << ex.what() << std::endl << "Usage: " << argv[0] << std::endl << desc << std::endl;
        exit(-1);
    }
    return vm;
}


int main(int argc, char *argv[]) {
    signal(SIGINT, signal_handler);
    po::variables_map args = Usage(argc, argv);
    boost::shared_ptr<Watchdog> watchdog(
            new Watchdog(boost::chrono::milliseconds(1000), args.count("auto"), args.count("verbose")));
    gWatchdog = watchdog;
    watchdog->setCallback(callBack);
    boost::shared_ptr<ThreadyMainWatchdogPolicy> threadyPolicy = boost::make_shared<ThreadyMainWatchdogPolicy>();
    watchdog->setPolicy(threadyPolicy);
    watchdog->monitor();
    std::cout << std::endl << "Finished!" << std::endl;
    return 0;
}
