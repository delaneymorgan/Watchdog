//
// Created by craig on 6/04/23.
//

#ifndef WATCHDOG_HEARTBEAT_H
#define WATCHDOG_HEARTBEAT_H


#include <boost/chrono.hpp>


enum HeartbeatEvent {
    Started_HeartbeatEvent,
    Abnormal_HeartbeatEvent,
    Fatal_HeartbeatEvent
};


struct Heartbeat {
    boost::chrono::milliseconds m_NormalLimit;
    boost::chrono::milliseconds m_AbsoluteLimit;
    boost::chrono::system_clock::time_point m_Beat;
    uint32_t m_CRC;


    Heartbeat() :
            m_NormalLimit( 0 ),
            m_AbsoluteLimit( 0 ),
            m_Beat(),
            m_CRC( 0 ) {}


    static void SetCRC( Heartbeat &heartbeat );
    static bool isCRCOK( const Heartbeat &heartbeat );
    static std::string makeActualName(const std::string &procName, const std::string &threadName);
    static std::string extractUserName( const std::string &actualName );
    static std::string extractProcName( const std::string &actualName );
    static std::string extractThreadName( const std::string &actualName );
    static pid_t extractProcessID( const std::string &actualName );
    static pid_t extractThreadID( const std::string &actualName );
    static bool isHeartbeat( const std::string &name );
    static uint32_t calcCRC( const Heartbeat &heartbeat );

    static std::string heartbeatEventName(const HeartbeatEvent event);
};


#endif //WATCHDOG_HEARTBEAT_H
