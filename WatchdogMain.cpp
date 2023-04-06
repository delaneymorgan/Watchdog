#include <iostream>
#include <csignal>

#include "EKG.h"

namespace {
    bool gRunning = true;
}

void signal_handler( int sig)
{
    if (sig == SIGINT)
    {
        gRunning = false;
    }
}

int main() {
    signal( SIGINT, signal_handler);
    while (gRunning)
    {
        try {
            EKG ekg( "FRED");
            while (gRunning)
            {
                try {
                    std::string alive = ekg.isAlive() ? "alive" : "dead";
                    std::cout << "Watching " << ekg.name() << " (" << ekg.threadID() << ") - " << alive << std::endl;
                }
                catch (const CorruptHeartbeat& ex)
                {
                    std::cout << "Corrupt heartbeat detected" << std::endl;
                }
                sleep( 1);
            }
        }
        catch (const boost::interprocess::interprocess_exception& ex)
        {
            std::cout << "Heartbeat " << "FRED" << " not present - retrying" << std::endl;
            sleep(1);
        }
    }
    std::cout << std::endl << "Finished!" << std::endl;
    return 0;
}
