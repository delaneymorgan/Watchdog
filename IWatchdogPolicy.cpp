/**
 * @file IWatchdogPolicy.cpp
 *
 * @brief an abstract class for managing events from the Watchdog class
 *
 * @copyright Delaney & Morgan Computing
 */

#include "IWatchdogPolicy.h"

#include <csignal>
#include <sys/reboot.h>
#include <sys/resource.h>
#include <sys/types.h>

IWatchdogPolicy::IWatchdogPolicy(const std::string &processName) :
        m_ProcessName(processName) {
}

IWatchdogPolicy::~IWatchdogPolicy() {
}

void IWatchdogPolicy::killProcess(pid_t processID) {
    int status = kill(processID, SIGRTMAX);
    if (status == -1) {
        throw KillError();
    }
}

void IWatchdogPolicy::setPriority(pid_t processID, TPriority priority) {
    if (!validPriority(priority)) {
        throw InvalidPriority();
    }
    int status = setpriority(PRIO_PROCESS, processID, priority);
    if (status == -1) {
        throw SetPriorityError();
    }
}

TPriority IWatchdogPolicy::getPriority(pid_t processID) {
    errno = 0;
    TPriority ret = getpriority(PRIO_PROCESS, processID);
    if (errno == -1) {
        throw GetPriorityError();
    }
    return ret;
}

void IWatchdogPolicy::rebootOS() {
    sync();
    reboot(RB_AUTOBOOT);
}

/**
 * returns the name of the policy's relevant process
 *
 * @return std::string the process name
 */
std::string IWatchdogPolicy::processName() const {
    return m_ProcessName;
}

bool IWatchdogPolicy::validPriority(TPriority priority) {
    if ((priority >= -20) && (priority < 19)) {
        return true;
    }
    return false;
}