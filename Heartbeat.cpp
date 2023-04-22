//
// Created by craig on 6/04/23.
//

#include "Heartbeat.h"

#include <boost/assign.hpp>
#include <boost/crc.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include <map>
#include <sstream>
#include <unistd.h>
#include <vector>

namespace {
    const std::string HEARTBEAT_PREFIX = "hb_";
    const boost::regex HEARTBEAT_REGEX("^hb_[A-Z|a-z|0-9|-]+\\:[A-Z|a-z|0-9|-]+\\.\\d+\\.\\d+$");
    std::map<HeartbeatEvent, std::string> HEARTBEATEVENT_TO_STRING = boost::assign::map_list_of
            (Started_HeartbeatEvent, "Started")
            (Slow_HeartbeatEvent, "Slow")
            (Hung_HeartbeatEvent, "Hung")
            (Dead_HeartbeatEvent, "Dead");
}

void Heartbeat::SetCRC(Heartbeat &heartbeat) {
    heartbeat.m_CRC = Heartbeat::calcCRC(heartbeat);
}

bool Heartbeat::isCRCOK(const Heartbeat &heartbeat) {
    uint32_t calcCRC = Heartbeat::calcCRC(heartbeat);
    if (heartbeat.m_CRC == calcCRC) {
        return true;
    }
    return false;
}

std::string Heartbeat::makeActualName(const std::string &procName, const std::string &threadName) {
    std::stringstream oss;
    pid_t processID = getpid();
    pid_t threadID = gettid();
    oss << HEARTBEAT_PREFIX << procName << ":" << threadName << "." << processID << "." << threadID;
    return oss.str();
}

std::string Heartbeat::extractUserName(const std::string &actualName) {
    std::vector<std::string> parts;
    boost::split(parts, actualName, boost::is_any_of("_."));
    return parts[1];
}

std::string Heartbeat::extractProcName(const std::string &actualName) {
    std::string userName = extractUserName(actualName);
    std::vector<std::string> parts;
    boost::split(parts, userName, boost::is_any_of(":"));
    return parts[0];
}

std::string Heartbeat::extractThreadName(const std::string &actualName) {
    std::string userName = extractUserName(actualName);
    std::vector<std::string> parts;
    boost::split(parts, userName, boost::is_any_of(":"));
    return parts[1];
}

pid_t Heartbeat::extractProcessID(const std::string &actualName) {
    std::vector<std::string> parts;
    boost::split(parts, actualName, boost::is_any_of("_."));
    std::string pidStr = parts[2];
    return boost::lexical_cast<pid_t>(pidStr);
}

pid_t Heartbeat::extractThreadID(const std::string &actualName) {
    std::vector<std::string> parts;
    boost::split(parts, actualName, boost::is_any_of("_."));
    std::string tidStr = parts[3];
    return boost::lexical_cast<pid_t>(tidStr);
}

bool Heartbeat::isHeartbeat(const std::string &name) {
    if (boost::regex_match(name, HEARTBEAT_REGEX)) {
        return true;
    }
    return false;
}

uint32_t Heartbeat::calcCRC(const Heartbeat &heartbeat) {
    boost::crc_32_type crc;
    crc.process_bytes(&heartbeat.m_Beat, sizeof(heartbeat.m_Beat));
    crc.process_bytes(&heartbeat.m_AbsoluteLimit, sizeof(heartbeat.m_AbsoluteLimit));
    crc.process_bytes(&heartbeat.m_NormalLimit, sizeof(heartbeat.m_NormalLimit));
    return crc.checksum();
}

std::string Heartbeat::heartbeatEventName(HeartbeatEvent event) {
    return HEARTBEATEVENT_TO_STRING[event];
}
