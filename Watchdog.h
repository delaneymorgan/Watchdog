/**
 * @file Watchdog.h
 *
 * @brief the Watchdog class used by a watchdog application to manage heartbeats
 *
 * @copyright Delaney & Morgan Computing
 */

#ifndef WATCHDOG_WATCHDOG_H
#define WATCHDOG_WATCHDOG_H


#include <map>
#include <unistd.h>
#include <boost/shared_ptr.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/chrono.hpp>
#include <boost/function.hpp>
#include <boost/filesystem.hpp>


#include "Heartbeat.h"
#include "EKG.h"
#include "IWatchdogPolicy.h"


namespace {

    struct TProcInfo {
        pid_t m_ProcessID;
        std::string m_ActualName;

        TProcInfo() :
                m_ProcessID(),
                m_ActualName() {
        }
    };

}


class IWatchdogPolicy;


class Watchdog {
public:
    Watchdog(boost::chrono::milliseconds scanPeriod, bool autoScan = false, bool verbose = false);
    virtual ~Watchdog();

    void setCallback(const boost::function<void(WatchdogEvent, bool)> &);
    void setPolicy(const boost::shared_ptr<IWatchdogPolicy> &policy);
    void monitor();
    void quiesce();

private:
    void scanHeartbeats();
    void doCallbacks(const WatchdogEvent &event);
    void doCallbacks(const boost::shared_ptr<EKG> &ekg, HeartbeatEvent event);

    boost::container::flat_set<std::string>
    static extractCandidateHBs(const std::vector<boost::filesystem::directory_entry> &dirList);

    boost::container::flat_set<std::string> compareAgainstEKGs(boost::container::flat_set<std::string> &candidateHBs);

    static std::map<std::string, TProcInfo> getDeadProcs(boost::container::flat_set<std::string> &deadPIDs);

    void notifyStakeholders(std::map<std::string, TProcInfo> &deadProcs);

    void cleanHeartbeats(boost::container::flat_set<std::string> &deadPIDs);

    std::map<std::string, boost::shared_ptr<EKG> > m_Heartbeats;
    std::map<std::string, boost::shared_ptr<IWatchdogPolicy> > m_Policies;
    bool m_Running;
    bool m_AutoScan;
    bool m_Verbose;
    boost::chrono::milliseconds m_ScanPeriod;
    boost::function<void(WatchdogEvent, bool)> m_CallBack;
};


#endif //WATCHDOG_WATCHDOG_H
