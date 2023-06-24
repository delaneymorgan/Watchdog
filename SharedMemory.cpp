/**
 * @file SharedMemory.cpp
 *
 * @brief this class encapsulates shared memory access
 *
 * @copyright Delaney & Morgan Computing
 */

#include "SharedMemory.h"

#include <boost/interprocess/shared_memory_object.hpp>


using namespace boost::interprocess;


SharedMemory::SharedMemory(const std::string &name) {
    shared_memory_object shm;
    shm = shared_memory_object(open_only, name.c_str(), boost::interprocess::read_only);
    m_Region = mapped_region(shm, read_only);
}

SharedMemory::SharedMemory(const std::string &name, size_t numBytes) {
    shared_memory_object shm;
    shm = shared_memory_object(open_or_create, name.c_str(), read_write);
    shm.truncate(numBytes);
    m_Region = mapped_region(shm, read_write);
}

SharedMemory::~SharedMemory() {
    // don't destroy region
}

void SharedMemory::write(void *source, size_t numBytes) {
    std::memcpy(m_Region.get_address(), reinterpret_cast<char *>( source), std::min(numBytes, m_Region.get_size()));
}

void SharedMemory::read(void *dest, size_t numBytes) {
    std::memcpy(reinterpret_cast<char *>(dest), m_Region.get_address(), std::min(numBytes, m_Region.get_size()));
}

void SharedMemory::remove(const std::string& name) {
    boost::interprocess::shared_memory_object::remove(name.c_str());
}