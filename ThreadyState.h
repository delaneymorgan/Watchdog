/**
 * @file ThreadyState.cpp
 *
 * @brief arbitrary state information for Thready.
 * Use something similar to check a client thread's context for any heartbeat events.
 *
 * @copyright Delaney & Morgan Computing
 */

#ifndef WATCHDOG_THREADYSTATE_H
#define WATCHDOG_THREADYSTATE_H

#include <string>


enum ThreadyState {
    NONE_ThreadyState,
    Happy_ThreadyState,
    Sad_ThreadyState
};

std::string threadyStateName(ThreadyState state);

#endif //WATCHDOG_THREADYSTATE_H
