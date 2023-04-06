cmake_minimum_required(VERSION 3.25)
project(Watchdog)

find_package(Boost COMPONENTS system filesystem thread chrono REQUIRED)

set(CMAKE_CXX_STANDARD 98)

add_executable(Watchdog
        WatchdogMain.cpp
        Watchdog.cpp
        Watchdog.h
        EKG.cpp
        EKG.h
        Heartbeat.h
        Heartbeat.cpp)

target_link_libraries(Watchdog
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
        ${Boost_CHRONO_LIBRARY}
        ${Boost_THREAD_LIBRARY})

add_executable(Thready
        Heartbeat.h
        Thready.cpp
        Thready.h
        ThreadyMain.cpp
        PaceMaker.h Heartbeat.cpp PaceMaker.cpp)

target_link_libraries(Thready
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
        ${Boost_CHRONO_LIBRARY}
        ${Boost_THREAD_LIBRARY})

