#include <iostream>
#include <csignal>
#include <boost/shared_ptr.hpp>
#include <boost/program_options.hpp>

#include "EKG.h"
#include "Watchdog.h"


boost::shared_ptr<Watchdog> gWatchdog;


void signal_handler( int sig ) {
    if ( sig == SIGINT ) {
        gWatchdog->quiesce();
    }
}


void callBack( std::string &actualName, pid_t processID, pid_t threadID, HeartbeatState state,
               boost::chrono::milliseconds hbLength, bool verbose ) {
    std::string stateName;
    switch ( state ) {
        case Abnormal_HeartbeatState:
            stateName = "abnormal";
            break;
        case Fatal_HeartbeatState:
            stateName = "fatal";
            break;
        default:
            break;
    }
    if (verbose) {
        if ( hbLength.count() == 0 ) {
            std::cout << "Process died: " << actualName << " - " << processID << std::endl;
        } else {
            std::cout << "Heartbeat " << stateName << ": " << actualName << " - " << processID << "/" <<
                      threadID << " = " << hbLength.count() << " msec" << std::endl;
        }
    }
}


namespace po = boost::program_options;
namespace postyle = boost::program_options::command_line_style;


po::variables_map Usage( int argc, char *argv[], const std::string &error = "" ) {
    po::options_description desc( "Options" );
    desc.add_options()
            ( "auto,a", "Auto-scale scan rate" )
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
    po::variables_map args = Usage( argc, argv );
    boost::shared_ptr<Watchdog> watchdog( new Watchdog( boost::chrono::milliseconds( 1000 ), args.count( "auto" )));
    gWatchdog = watchdog;
    watchdog->setCallback( callBack );
    watchdog->monitor();
    std::cout << std::endl << "Finished!" << std::endl;
    return 0;
}
