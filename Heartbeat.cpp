//
// Created by craig on 6/04/23.
//

#include "Heartbeat.h"

#include <boost/crc.hpp>

void Heartbeat::SetCRC(Heartbeat &heartbeat) {
    boost::crc_32_type crc;
    crc.process_bytes( &heartbeat.m_Beat, sizeof( heartbeat.m_Beat));
    crc.process_bytes( &heartbeat.m_Limit, sizeof( heartbeat.m_Limit));
    crc.process_bytes( &heartbeat.m_ThreadID, sizeof( heartbeat.m_ThreadID));
    heartbeat.m_CRC = crc.checksum();
}

bool Heartbeat::isCRCOK(const Heartbeat &heartbeat) {
    boost::crc_32_type crc;
    crc.process_bytes( &heartbeat.m_Beat, sizeof( heartbeat.m_Beat));
    crc.process_bytes( &heartbeat.m_Limit, sizeof( heartbeat.m_Limit));
    crc.process_bytes( &heartbeat.m_ThreadID, sizeof( heartbeat.m_ThreadID));
    if (heartbeat.m_CRC == crc.checksum())
    {
        return true;
    }
    return false;
}