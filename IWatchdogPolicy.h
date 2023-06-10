/**
 * @file IWatchdogPolicy.h
 *
 * @brief an abstract class for managing events from the Watchdog class
 *
 * @copyright Delaney & Morgan Computing
 */

#ifndef WATCHDOG_IWATCHDOGPOLICY_H
#define WATCHDOG_IWATCHDOGPOLICY_H

#include <string>

#include <boost/chrono.hpp>

#include "Heartbeat.h"
#include "WatchdogEvent.h"


class IWatchdogPolicy {
public:
    IWatchdogPolicy(const std::string &processName);
    virtual ~IWatchdogPolicy();

    std::string processName() const;
    virtual void handleEvent(const WatchdogEvent &event, bool verbose) = 0;

protected:
    std::string m_ProcessName;
};


#endif //WATCHDOG_IWATCHDOGPOLICY_H
