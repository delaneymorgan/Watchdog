//
// Created by craig on 6/04/23.
//

#ifndef WATCHDOG_WATCHDOG_H
#define WATCHDOG_WATCHDOG_H

#include <map>
#include <unistd.h>
#include <boost/shared_ptr.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/chrono.hpp>
#include <boost/function.hpp>

#include "EKG.h"

class Watchdog {
public:
    Watchdog( boost::chrono::milliseconds scanPeriod);
    virtual ~Watchdog();

    void setCallback(boost::function<void(std::string&, pid_t, pid_t)>);
    void monitor();
    void quiesce();
private:
    void scanHeartbeats();

    std::map<std::string, boost::shared_ptr<EKG> > m_Heartbeats;
    bool m_Running;
    boost::chrono::milliseconds m_ScanPeriod;
    boost::function<void(std::string, pid_t, pid_t)> m_CallBack;
};


#endif //WATCHDOG_WATCHDOG_H
