/**
 * @file ThreadyMain.cpp
 *
 * @brief this is an example program demonstrating the structure of a Watchdog client application.
 * It creates the specified number of instnaces of Thready, each with it's own Heartbeat, managed via the PaceMaker
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
    const boost::chrono::milliseconds NORMAL_LIMIT( 500 );
    const boost::chrono::milliseconds ABSOLUTE_LIMIT( 1000 );
}


void signal_handler( int sig ) {
    if ( sig == SIGINT ) {
        gRunning = false;
    }
}


namespace po = boost::program_options;
namespace postyle = boost::program_options::command_line_style;


po::variables_map Usage(int argc, char *argv[]) {
    po::options_description desc( "Options" );
    desc.add_options()
            ( "num,n", po::value<unsigned int>()->default_value(20), "The #threads" )
            ( "tamper,t", "Tamper with heartbeats" )
            ( "verbose,v", "Verbose mode" )
            ( "help,?", "Print this help message" );
    po::variables_map vm;
    try {
        po::store( po::parse_command_line( argc, argv, desc ), vm );
        po::notify( vm );
        if ( vm.count( "help" )) {
            std::cout << "Usage: " << argv[0] << std::endl << desc << std::endl;
            exit( 0 );
        }
    }
    catch ( po::error &ex ) {
        std::cerr << ex.what() << std::endl << "Usage: " << argv[0] << std::endl << desc << std::endl;
        exit( -1 );
    }
    return vm;
}


int main( int argc, char *argv[] ) {
    signal( SIGINT, signal_handler );
    po::variables_map args = Usage(argc, argv);
    std::vector<boost::shared_ptr<Thready> > threadys;
    unsigned int numThreads = args["num"].as<unsigned int>();
    for ( int threadNo = 0; threadNo < numThreads; threadNo++ ) {
        boost::shared_ptr<Thready> newThready(new Thready("ThreadyMain", threadNo, NORMAL_LIMIT, ABSOLUTE_LIMIT,
                                                          args.count("tamper"), args.count("verbose")));
        __attribute__((unused)) boost::thread *thr = new boost::thread( boost::bind( &Thready::run, newThready ));
        threadys.push_back( newThready );
    }
    while ( gRunning ) {
        if (args.count("verbose")) {
            std::cout << "Idle loop" << std::endl;
        }
        sleep( 1 );
    }
    for ( TThreadysIterator it = threadys.begin(); it != threadys.end(); it++ ) {
        (*it)->quiesce();
    }
    std::cout << std::endl << "Finished!" << std::endl;
}