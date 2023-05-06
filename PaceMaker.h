/**
 * @file PaceMaker.h
 *
 * @brief the PaceMaker class is used by a Watchdog client to manage the heartbeat for a single thread.
 *
 * @copyright Delaney & Morgan Computing
 */

#ifndef WATCHDOG_PACEMAKER_H
#define WATCHDOG_PACEMAKER_H


#include <string>

#include <boost/chrono.hpp>
#include <boost/thread.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/chrono/duration.hpp>
#include <boost/chrono/chrono.hpp>


class InvalidHeartbeatName :
        public std::exception {
};


class PaceMaker {
public:
    PaceMaker(const std::string &procName, const std::string &threadName, boost::chrono::milliseconds normalLimit,
              boost::chrono::milliseconds absoluteLimit);
    virtual ~PaceMaker();

    void pulse();

private:
    const std::string m_ProcName;
    const std::string m_ThreadName;
    const std::string m_ActualName;
    boost::chrono::milliseconds m_NormalLimit;
    boost::chrono::milliseconds m_AbsoluteLimit;
    boost::mutex m_BeatMutex;
    boost::interprocess::mapped_region m_Region;
};


#endif //WATCHDOG_PACEMAKER_H
