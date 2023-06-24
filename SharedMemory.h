/**
 * @file SharedMemory.h
 *
 * @brief this class encapsulates shared memory access
 *
 * @copyright Delaney & Morgan Computing
 */

#ifndef WATCHDOG_SHAREDMEMORY_H
#define WATCHDOG_SHAREDMEMORY_H

#include <boost/interprocess/mapped_region.hpp>

#include <string>


class SharedMemory {
public:
    SharedMemory(const std::string &name);
    SharedMemory(const std::string &name, size_t numBytes);
    virtual ~SharedMemory();

    void write(void *source, size_t numBytes);
    void read(void *dest, size_t numBytes);
    static void remove(const std::string& name);

private:
    boost::interprocess::mapped_region m_Region;    // the actual region in shared memory for this heartbeat
};


#endif //WATCHDOG_SHAREDMEMORY_H
