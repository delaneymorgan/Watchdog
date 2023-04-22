//
// Created by craig on 6/04/23.
//

#ifndef WATCHDOG_EKG_H
#define WATCHDOG_EKG_H


#include <exception>
#include <string>

#include <boost/interprocess/mapped_region.hpp>
#include <boost/chrono.hpp>


class CorruptHeartbeat :
        public std::exception {
};


class EKG {
public:
    explicit EKG( const std::string &actualName );
    virtual ~EKG();

    bool isAlive();
    bool isNormal();
    boost::chrono::milliseconds length();

    std::string actualName();
    boost::chrono::milliseconds normalLimit();
    pid_t processID() const;
    pid_t threadID() const;

private:
    const std::string m_ActualName;
    const std::string m_UserName;
    pid_t m_ProcessID;
    pid_t m_ThreadID;
    boost::interprocess::mapped_region m_Region;
};

#endif //WATCHDOG_EKG_H
