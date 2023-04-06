//
// Created by craig on 6/04/23.
//

#ifndef WATCHDOG_PACEMAKER_H
#define WATCHDOG_PACEMAKER_H


#include <string>

#include <boost/chrono.hpp>
#include <boost/thread.hpp>
#include <boost/interprocess/mapped_region.hpp>


class PaceMaker {
public:
    PaceMaker( const std::string& heartbeatName, boost::chrono::milliseconds heartbeatLimit);
    virtual ~PaceMaker();

    void beat();
    std::string name();
private:
    const std::string m_Name;
    boost::chrono::milliseconds m_Limit;
    boost::mutex m_BeatMutex;
    boost::interprocess::mapped_region m_Region;
};


#endif //WATCHDOG_PACEMAKER_H
