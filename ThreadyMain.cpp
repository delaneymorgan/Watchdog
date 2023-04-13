//
// Created by craig on 6/04/23.
//

#include "Thready.h"

#include <iostream>
#include <csignal>
#include <unistd.h>
#include <string>

#include "PaceMaker.h"


namespace {
    const unsigned int NUM_THREADS = 20;
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

int main() {
    std::vector<boost::shared_ptr<Thready> > threadys;
    signal(SIGINT, signal_handler);
    for (int threadNo = 0; threadNo < NUM_THREADS; threadNo++) {
        boost::shared_ptr<Thready> newThready( new Thready(threadNo,NORMAL_LIMIT,ABSOLUTE_LIMIT));
        boost::thread* thr = new boost::thread(boost::bind(&Thready::run, newThready));
        threadys.push_back(newThready);
    }
    while (gRunning) {
        std::cout << "Idle loop" << std::endl;
        sleep(1);
    }
    for (TThreadysIterator it = threadys.begin(); it != threadys.end(); it++) {
        (*it)->quiesce();
    }
    std::cout << std::endl << "Finished!" << std::endl;
}