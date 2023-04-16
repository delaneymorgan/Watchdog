#include <iostream>
#include <csignal>
#include <boost/shared_ptr.hpp>
#include <boost/program_options.hpp>

#include "EKG.h"
#include "Watchdog.h"


boost::shared_ptr<Watchdog> gWatchdog;


void signal_handler(int sig) {
    if (sig == SIGINT) {
        gWatchdog->quiesce();
    }
}


/**
 * callback for Watchdog - any events will be reported here
 *
 * @param actualName the full name of the heartbeat
 * @param processID the process id of the monitored process
 * @param threadID the thread id within the monitored process
 * @param event - event description
 * @param hbLength the length of the heartbeat that triggered the event
 * @param verbose true => verbose, false => quiet
 */
void callBack(std::string &actualName, pid_t processID, pid_t threadID, HeartbeatEvent event,
              boost::chrono::milliseconds hbLength, bool verbose) {
    std::string eventName = Heartbeat::heartbeatEventName(event);
    if (verbose) {
        switch (event) {
            case Started_HeartbeatEvent:
                std::cout << "Heartbeat " << eventName << ": " << Heartbeat::extractProcName(actualName) <<
                          ":" << Heartbeat::extractThreadName(actualName) << " - " << processID << "/" <<
                          threadID << std::endl;
                break;

            case Abnormal_HeartbeatEvent:
                std::cout << "Heartbeat " << eventName << ": " << Heartbeat::extractProcName(actualName) <<
                          ":" << Heartbeat::extractThreadName(actualName) << " - " << processID << "/" <<
                          threadID << " = " << hbLength.count() << " mSec" << std::endl;
                break;

            case Fatal_HeartbeatEvent:
                std::cout << "Process died: " << Heartbeat::extractProcName(actualName) << " - " << processID
                          << std::endl;
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
    watchdog->monitor();
    std::cout << std::endl << "Finished!" << std::endl;
    return 0;
}
