//
// Created by craig on 6/04/23.
//

#ifndef WATCHDOG_HEARTBEAT_H
#define WATCHDOG_HEARTBEAT_H

#include <boost/chrono.hpp>


struct Heartbeat {
    boost::chrono::milliseconds m_Limit;
    boost::chrono::system_clock::time_point m_Beat;
    pid_t m_ThreadID;
    uint32_t m_CRC;

    Heartbeat() :
        m_ThreadID(),
        m_CRC(0)
    {}

    static void SetCRC( Heartbeat &heartbeat);
    static bool isCRCOK( const Heartbeat& heartbeat);

};


#endif //WATCHDOG_HEARTBEAT_H
