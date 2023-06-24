/**
 * @file EKG.h
 *
 * @brief the EKG class used by the Watchdog class to manage a single heartbeat
 *
 * @copyright Delaney & Morgan Computing
 */

#ifndef WATCHDOG_EKG_H
#define WATCHDOG_EKG_H

#include "SharedMemory.h"

#include <exception>
#include <string>

#include <boost/chrono.hpp>


class CorruptHeartbeat :
        public std::exception {
};


class EKG {
public:
    explicit EKG(const std::string &actualName);
    virtual ~EKG();

    bool isAlive();
    bool isNormal();
    boost::chrono::milliseconds length();
    int info();
    std::string actualName();
    boost::chrono::milliseconds normalLimit();
    pid_t processID() const;
    pid_t threadID() const;

private:
    const std::string m_ActualName;     // the name of the region as it appears in /dev/shm
    pid_t m_ProcessID;  // the heartbeat's process id
    pid_t m_ThreadID;   // the heartbeat's thread id
    SharedMemory m_SharedMemory;    // the shared memory object for this heartbeat
};

#endif //WATCHDOG_EKG_H
