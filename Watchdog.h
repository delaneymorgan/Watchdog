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
#include "IWatchdogPolicy.h"


class IWatchdogPolicy;


class Watchdog {
public:
    Watchdog( boost::chrono::milliseconds scanPeriod, bool autoScan = false, bool verbose = false );
    virtual ~Watchdog();

    void setCallback( const boost::function<void(std::string &, pid_t, pid_t, HeartbeatEvent, boost::chrono::milliseconds, bool )>&);
    void setPolicy( const boost::shared_ptr<IWatchdogPolicy>& policy);
    void monitor();
    void quiesce();
private:
    void scanHeartbeats();
    void doCallbacks(std::string &actualName, pid_t processID, pid_t threadID, HeartbeatEvent event,
                     boost::chrono::milliseconds hbLength);
    void doCallbacks(const boost::shared_ptr<EKG>& ekg, HeartbeatEvent event);

    std::map<std::string, boost::shared_ptr<EKG> > m_Heartbeats;
    std::map<std::string, boost::shared_ptr<IWatchdogPolicy> > m_Policies;
    bool m_Running;
    bool m_AutoScan;
    bool m_Verbose;
    boost::chrono::milliseconds m_ScanPeriod;
    boost::function<void(std::string, pid_t, pid_t, HeartbeatEvent, boost::chrono::milliseconds, bool )> m_CallBack;
};


#endif //WATCHDOG_WATCHDOG_H
