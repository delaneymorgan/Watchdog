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

enum ThreadyState {
    NONE_ThreadyState,
    Happy_ThreadyState,
    Sad_ThreadyState
};

class Thready {
public:
    Thready(const std::string &procName, unsigned int id, boost::chrono::milliseconds normalLimit,
            boost::chrono::milliseconds absoluteLimit, bool tamper, bool verbose);
    virtual ~Thready();

    void run();
    void quiesce();

private:
    std::string m_ProcName;
    std::string m_ThreadName;
    bool m_Running;
    bool m_Tamper;
    bool m_Verbose;
    boost::shared_ptr<PaceMaker> m_PaceMaker;
    boost::chrono::milliseconds m_NormalLimit;
    boost::chrono::milliseconds m_AbsoluteLimit;
};


#endif //WATCHDOG_THREADY_H
