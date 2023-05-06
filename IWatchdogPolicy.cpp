/**
 * @file IWatchdogPolicy.cpp
 *
 * @brief an abstract class for managing events from the Watchdog class
 *
 * @copyright Delaney & Morgan Computing
 */

#include "IWatchdogPolicy.h"

IWatchdogPolicy::IWatchdogPolicy(const std::string &processName) :
        m_ProcessName(processName) {
}

IWatchdogPolicy::~IWatchdogPolicy() {
}

/**
 * returns the name of the policy's relevant process
 *
 * @return std::string the process name
 */
std::string IWatchdogPolicy::processName() const {
    return m_ProcessName;
}