//
// Created by craig on 6/04/23.
//

#ifndef WATCHDOG_THREADY_H
#define WATCHDOG_THREADY_H


#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

#include "PaceMaker.h"


class Thready {
public:
    Thready( unsigned int id, boost::chrono::milliseconds normalLimit, boost::chrono::milliseconds absoluteLimit, bool tamper, bool verbose);
    virtual ~Thready();
    void run();
    void quiesce();
private:
    std::string m_Name;
    bool m_Running;
    bool m_Tamper;
    bool m_Verbose;
    boost::shared_ptr<PaceMaker> m_PaceMaker;
    boost::chrono::milliseconds m_NormalLimit;
    boost::chrono::milliseconds m_AbsoluteLimit;
};


#endif //WATCHDOG_THREADY_H
