//
// Created by craig on 6/04/23.
//

#include "Thready.h"

#include <iostream>
#include <sstream>

#define QUOTE(x) #x

Thready::Thready(unsigned int id, boost::chrono::milliseconds normalLimit, boost::chrono::milliseconds absoluteLimit) :
        m_Running(true),
        m_PaceMaker(),
        m_NormalLimit(normalLimit),
        m_AbsoluteLimit(absoluteLimit) {
    std::stringstream oss;
    oss << QUOTE(Thready) << "-" << id;
    m_Name = oss.str();
}

Thready::~Thready() {

}

void Thready::run() {
    // creat pacemaker here to get thread id
    m_PaceMaker = boost::shared_ptr<PaceMaker>( new PaceMaker(m_Name, m_NormalLimit, m_AbsoluteLimit));
    while (m_Running) {
        m_PaceMaker->beat();
        std::stringstream oss;
        oss << m_Name << " beating" << std::endl;
        std::cout << oss.str();
        boost::chrono::milliseconds sleepTime = m_NormalLimit;
        long meanTime = (m_NormalLimit.count() + m_AbsoluteLimit.count()) / 2.0;
        long variance = (m_AbsoluteLimit.count() - m_NormalLimit.count()) / 2.0 * 1.2;
        // TODO: need random sleep between meantime-variance to meantime+variance
        // TODO: make irregular beat
        boost::this_thread::sleep_for(sleepTime);
    }
}

void Thready::quiesce() {
    m_Running = false;
}