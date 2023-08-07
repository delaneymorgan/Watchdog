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

typedef int TPriority;

class KillError : public std::exception {
};

class GetPriorityError : public std::exception {
};

class SetPriorityError : public std::exception {
};

class InvalidPriority : public std::exception {
};

class IWatchdogPolicy {
public:
    IWatchdogPolicy(const std::string &processName);
    virtual ~IWatchdogPolicy();

    std::string processName() const;
    virtual void handleEvent(const WatchdogEvent &event, bool verbose) = 0;
    static void killProcess( pid_t processID);
    static void setPriority( pid_t processID, TPriority priority);
    static TPriority getPriority( pid_t processID);
    static void rebootOS();

protected:
    std::string m_ProcessName;
private:
    static bool validPriority( TPriority priority);
};


#endif //WATCHDOG_IWATCHDOGPOLICY_H
