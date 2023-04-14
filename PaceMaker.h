//
// Created by craig on 6/04/23.
//

#ifndef WATCHDOG_PACEMAKER_H
#define WATCHDOG_PACEMAKER_H


#include <string>

#include <boost/chrono.hpp>
#include <boost/thread.hpp>
#include <boost/interprocess/mapped_region.hpp>


class InvalidHeartbeatName :
        public std::exception {
};


class PaceMaker {
public:
    PaceMaker( const std::string &userName, boost::chrono::milliseconds normalLimit,
               boost::chrono::milliseconds absoluteLimit );
    virtual ~PaceMaker();

    void beat();
    std::string userName();
    std::string actualName();
    pid_t processID() const;
    pid_t threadID() const;
private:
    const std::string m_UserName;
    const std::string m_ActualName;
    boost::chrono::milliseconds m_NormalLimit;
    boost::chrono::milliseconds m_AbsoluteLimit;
    boost::mutex m_BeatMutex;
    boost::interprocess::mapped_region m_Region;
    pid_t m_ProcessID;
    pid_t m_ThreadID;
};


#endif //WATCHDOG_PACEMAKER_H
