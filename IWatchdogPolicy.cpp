//
// Created by craig on 22/04/23.
//

#include "IWatchdogPolicy.h"

IWatchdogPolicy::IWatchdogPolicy(const std::string &processName) :
        m_ProcessName(processName) {
}

IWatchdogPolicy::~IWatchdogPolicy() {
}

std::string IWatchdogPolicy::processName() const {
    return m_ProcessName;
}