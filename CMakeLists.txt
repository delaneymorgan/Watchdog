cmake_minimum_required(VERSION 3.25)
project(Watchdog)

find_package(Boost COMPONENTS system filesystem thread chrono regex program_options REQUIRED)

set(CMAKE_CXX_STANDARD 98)

add_executable(WatchdogMain
        WatchdogMain.cpp
        Watchdog.cpp
        Watchdog.h
        EKG.cpp
        EKG.h
        Heartbeat.h
        Heartbeat.cpp
        IWatchdogPolicy.cpp
        IWatchdogPolicy.h
        ThreadyMainWatchdogPolicy.cpp
        ThreadyMainWatchdogPolicy.h macros.h ThreadyState.h ThreadyState.cpp WatchdogEvent.cpp WatchdogEvent.h SharedMemory.cpp SharedMemory.h)

target_link_libraries(WatchdogMain
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
        ${Boost_CHRONO_LIBRARY}
        ${Boost_THREAD_LIBRARY}
        ${Boost_REGEX_LIBRARY}
        ${Boost_PROGRAM_OPTIONS_LIBRARY}
        rt)

add_executable(ThreadyMain
        ThreadyMain.cpp
        Heartbeat.h
        Heartbeat.cpp
        Thready.cpp
        Thready.h
        PaceMaker.h
        PaceMaker.cpp ThreadyState.h ThreadyState.cpp SharedMemory.cpp SharedMemory.h)

target_link_libraries(ThreadyMain
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
        ${Boost_CHRONO_LIBRARY}
        ${Boost_THREAD_LIBRARY}
        ${Boost_REGEX_LIBRARY}
        ${Boost_PROGRAM_OPTIONS_LIBRARY}
        rt)

