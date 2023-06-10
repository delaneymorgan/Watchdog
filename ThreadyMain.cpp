/**
 * @file ThreadyMain.cpp
 *
 * @brief this is an example program demonstrating the structure of a Watchdog client application.
 * It creates the specified number of instances of Thready, each with it's own Heartbeat, managed via the PaceMaker
 * class.
 *
 * @copyright Delaney & Morgan Computing
 */

#include "Thready.h"

#include <iostream>
#include <csignal>
#include <unistd.h>
#include <string>

#include <boost/program_options.hpp>

#include "PaceMaker.h"


namespace {
    bool gRunning = true;
    typedef std::vector<boost::shared_ptr<Thready> >::iterator TThreadysIterator;
    const boost::chrono::milliseconds NORMAL_LIMIT(500);
    const boost::chrono::milliseconds ABSOLUTE_LIMIT(1000);
}


void signal_handler(int sig) {
    if (sig == SIGINT) {
        gRunning = false;
    }
}


namespace po = boost::program_options;
namespace postyle = boost::program_options::command_line_style;


po::variables_map Usage(int argc, char *argv[]) {
    po::options_description desc("Options");
    desc.add_options()
            ("num,n", po::value<unsigned int>()->default_value(20), "The #threads (1-500)")
            ("tamper,t", "Tamper with heartbeats")
            ("verbose,v", "Verbose mode")
            ("help,?", "Print this help message");
    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
        unsigned int num = vm["num"].as<unsigned int>();
        if ((num > 500) || (num < 1)) {
            std::cout << "Error: Too many threads" << std::endl << std::endl;
            std::cout << "Usage: " << argv[0] << std::endl << desc << std::endl;
            exit(0);
        }
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
    std::vector<boost::shared_ptr<Thready> > threadys;
    std::vector<boost::shared_ptr<boost::thread> > threads;
    unsigned int numThreads = args["num"].as<unsigned int>();
    for (int threadNo = 0; threadNo < numThreads; threadNo++) {
        boost::shared_ptr<Thready> newThready(new Thready("ThreadyMain", threadNo, NORMAL_LIMIT, ABSOLUTE_LIMIT,
                                                          args.count("tamper"), args.count("verbose")));
        boost::shared_ptr<boost::thread> thr = boost::shared_ptr<boost::thread>(
                new boost::thread(boost::bind(&Thready::run, newThready)));
        threadys.push_back(newThready);
        threads.push_back(thr);
    }
    while (gRunning) {
        if (args.count("verbose")) {
            std::cout << "Idle loop" << std::endl;
        }
        sleep(1);
    }
    for (TThreadysIterator it = threadys.begin(); it != threadys.end(); it++) {
        (*it)->quiesce();
    }
    for (std::vector<boost::shared_ptr<boost::thread> >::iterator it = threads.begin(); it != threads.end(); it++) {
        (*it)->join();
    }
    std::cout << std::endl << "Finished!" << std::endl;
}