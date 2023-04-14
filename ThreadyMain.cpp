//
// Created by craig on 6/04/23.
//

#include "Thready.h"

#include <iostream>
#include <csignal>
#include <unistd.h>
#include <string>

#include <boost/program_options.hpp>

#include "PaceMaker.h"


namespace {
    const unsigned int NUM_THREADS = 20;
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


po::variables_map Usage( int argc, char *argv[], const std::string &error = "" ) {
    po::options_description desc( "Options" );
    desc.add_options()
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
    po::variables_map args = Usage( argc, argv );
    std::vector<boost::shared_ptr<Thready> > threadys;
    for ( int threadNo = 0; threadNo < NUM_THREADS; threadNo++ ) {
        boost::shared_ptr<Thready> newThready( new Thready( threadNo, NORMAL_LIMIT, ABSOLUTE_LIMIT ));
        boost::thread *thr = new boost::thread( boost::bind( &Thready::run, newThready ));
        threadys.push_back( newThready );
    }
    while ( gRunning ) {
        std::cout << "Idle loop" << std::endl;
        sleep( 1 );
    }
    for ( TThreadysIterator it = threadys.begin(); it != threadys.end(); it++ ) {
        (*it)->quiesce();
    }
    std::cout << std::endl << "Finished!" << std::endl;
}