/**
 * @file Thready.h
 *
 * @brief this is an example class which creates a single thread and an associated PaceMaker instance.
 * The example program ThreadyMain creates n instances of this class.
 *
 * @copyright Delaney & Morgan Computing
 */

#ifndef WATCHDOG_THREADY_H
#define WATCHDOG_THREADY_H


#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

#include "PaceMaker.h"


class Thready {
public:
    Thready(const std::string &processName, unsigned int id, boost::chrono::milliseconds normalLimit,
            boost::chrono::milliseconds absoluteLimit, bool tamper, bool verbose);
    virtual ~Thready();

    void start();
    void quiesce();

private:
    void run();

    std::string m_ProcessName;
    std::string m_ThreadName;
    bool m_Running;
    bool m_Tamper;
    bool m_Verbose;
    boost::shared_ptr<PaceMaker> m_PaceMaker;
    boost::chrono::milliseconds m_NormalLimit;
    boost::chrono::milliseconds m_AbsoluteLimit;
    boost::shared_ptr<boost::thread> m_Thread;
};


#endif //WATCHDOG_THREADY_H
