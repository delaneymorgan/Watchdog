//
// Created by craig on 6/04/23.
//

#ifndef WATCHDOG_EKG_H
#define WATCHDOG_EKG_H

#include <exception>
#include <string>

#include <boost/interprocess/mapped_region.hpp>


class CorruptHeartbeat :
public std::exception
{
};

class EKG {
public:
    explicit EKG( const std::string& heartbeatName);
    virtual ~EKG();

    bool isAlive();
    std::string name();
    pid_t threadID();

private:
    const std::string m_Name;
    boost::interprocess::mapped_region m_Region;
};


#endif //WATCHDOG_EKG_H
