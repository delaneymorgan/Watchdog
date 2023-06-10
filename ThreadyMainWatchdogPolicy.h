/**
 * @file ThreadyMainWatchdogPolicy.h
 *
 * @brief a specific watchdog policy for the ThreadyMain application
 *
 * @copyright Delaney & Morgan Computing
 */


#ifndef WATCHDOG_THREADYMAIN_WATCHDOG_POLICY_H
#define WATCHDOG_THREADYMAIN_WATCHDOG_POLICY_H

#include "IWatchdogPolicy.h"


class ThreadyMainWatchdogPolicy : public IWatchdogPolicy {
public:
    ThreadyMainWatchdogPolicy();
    ~ThreadyMainWatchdogPolicy();

    virtual void handleEvent(const WatchdogEvent &event, bool verbose);

};


#endif //WATCHDOG_THREADYMAIN_WATCHDOG_POLICY_H
