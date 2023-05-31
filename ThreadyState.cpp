/**
 * @file ThreadyState.cpp
 *
 * @brief arbitrary state information for Thready.
 * Use something similar to check a client thread's context for any heartbeat events.
 *
 * @copyright Delaney & Morgan Computing
 */

#include "ThreadyState.h"

#include <map>

#include <boost/assign.hpp>

namespace {
    std::map<ThreadyState, std::string> THREADY_STATE_TO_STRING = boost::assign::map_list_of
            (NONE_ThreadyState, "")
            (Happy_ThreadyState, "Happy")
            (Sad_ThreadyState, "Sad");

    bool IsValidThreadyState(ThreadyState state) {
        if (state <= Sad_ThreadyState) {
            return true;
        }
        return false;
    }
}

std::string threadyStateName(ThreadyState state) {
    if (IsValidThreadyState(state)) {
        return THREADY_STATE_TO_STRING[state];
    }
    return "";
}

