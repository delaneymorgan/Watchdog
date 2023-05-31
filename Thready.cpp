/**
 * @file Thready.cpp
 *
 * @brief this is an example class which creates a single thread and an associated PaceMaker instance.
 * The example program ThreadyMain creates n instances of this class.
 *
 * @copyright Delaney & Morgan Computing
 */

#include "Thready.h"

#include "ThreadyState.h"

#include <iostream>
#include <sstream>

#include <boost/random.hpp>


#define QUOTE(x) #x


Thready::Thready(const std::string &procName, unsigned int id, boost::chrono::milliseconds normalLimit,
                 boost::chrono::milliseconds absoluteLimit, bool tamper, bool verbose)
        :
        m_ProcName(procName),
        m_Running(true),
        m_Tamper(tamper),
        m_Verbose(verbose),
        m_PaceMaker(),
        m_NormalLimit(normalLimit),
        m_AbsoluteLimit(absoluteLimit) {
    std::stringstream oss;
    oss << QUOTE(Thready) << "-" << id;
    m_ThreadName = oss.str();
}


Thready::~Thready() {

}


void Thready::run() {
    typedef boost::mt19937 TRNG;
    // create pacemaker here to get thread id
    m_PaceMaker = boost::shared_ptr<PaceMaker>(new PaceMaker(m_ProcName, m_ThreadName, m_NormalLimit, m_AbsoluteLimit));
    while (m_Running) {
        m_PaceMaker->pulse(Happy_ThreadyState);
        std::stringstream oss;
        if (m_Verbose) {
            oss << m_ThreadName << " beating" << std::endl;
        }
        std::cout << oss.str();
        boost::chrono::milliseconds sleepTime = m_NormalLimit;
        if (m_Tamper) {
            TRNG rng;
            int minTime = static_cast<int>(m_NormalLimit.count() / 2);
            int maxTime = static_cast<int>(static_cast<int>(m_AbsoluteLimit.count()) * 1.002);
            boost::uniform_int<> variance(minTime, maxTime);
            boost::variate_generator<TRNG, boost::uniform_int<> > dice(rng, variance);
            sleepTime = boost::chrono::milliseconds(dice());
        }
        boost::this_thread::sleep_for(sleepTime);
    }
}


void Thready::quiesce() {
    m_Running = false;
}