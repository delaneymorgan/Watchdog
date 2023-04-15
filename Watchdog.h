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

#include "Heartbeat.h"
#include "EKG.h"


class Watchdog {
public:
    Watchdog( boost::chrono::milliseconds scanPeriod, bool autoScan = false,
              bool verbose = false );      // TODO: option to scale to shortest heartbeat?
    virtual ~Watchdog();

    void setCallback( boost::function<void( std::string &, pid_t, pid_t, HeartbeatState, boost::chrono::milliseconds, bool )>);
    void monitor();
    void quiesce();
private:
    void scanHeartbeats();

    std::map<std::string, boost::shared_ptr<EKG> > m_Heartbeats;
    bool m_Running;
    bool m_AutoScan;
    bool m_Verbose;
    boost::chrono::milliseconds m_ScanPeriod;
    boost::function<void( std::string, pid_t, pid_t, HeartbeatState, boost::chrono::milliseconds, bool )> m_CallBack;
};


#endif //WATCHDOG_WATCHDOG_H
