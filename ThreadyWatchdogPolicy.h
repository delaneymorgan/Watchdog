//
// Created by craig on 22/04/23.
//

#ifndef WATCHDOG_THREADY_WATCHDOG_POLICY_H
#define WATCHDOG_THREADY_WATCHDOG_POLICY_H

#include "IWatchdogPolicy.h"


class ThreadyWatchdogPolicy : public IWatchdogPolicy {
public:
    ThreadyWatchdogPolicy();
    ~ThreadyWatchdogPolicy();

    virtual void handleEvent(std::basic_string<char> actualName, pid_t processID, pid_t threadID, HeartbeatEvent event,
                             boost::chrono::milliseconds hbLength, bool verbose);

};


#endif //WATCHDOG_THREADY_WATCHDOG_POLICY_H
