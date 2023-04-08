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
    bool gRunning = true;
}

void signal_handler(int sig) {
    if (sig == SIGINT) {
        gRunning = false;
    }
}

int main() {
    signal(SIGINT, signal_handler);
    PaceMaker pm("FRED", boost::chrono::milliseconds(200), boost::chrono::milliseconds(1000));
    while (gRunning) {
        pm.beat();
        std::cout << "Beating " << pm.userName() << " (" << pm.processID() << "/" << pm.threadID() << ")" << std::endl;
        sleep(1);
    }
    std::cout << std::endl << "Finished!" << std::endl;
}