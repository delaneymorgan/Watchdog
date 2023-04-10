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
    m_PaceMaker = boost::shared_ptr<PaceMaker>( new PaceMaker(m_Name, normalLimit, absoluteLimit));
}

Thready::~Thready() {

}

void Thready::run() {
    while (m_Running) {
        m_PaceMaker->beat();
        std::stringstream oss;
        oss << m_Name << " beating" << std::endl;
        std::cout << oss.str();
        boost::chrono::milliseconds sleepTime = m_NormalLimit;      // make irregular beat
        boost::this_thread::sleep_for(sleepTime);
    }
}

void Thready::quiesce() {
    m_Running = false;
}