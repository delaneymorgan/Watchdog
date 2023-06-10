# Watchdog

C++98/Boost Watchdog classes Using Shared Memory

There are two sample applications:

* WatchdogMain - an example implementation of a watchdog
* ThreadyMain - a client of the watchdog

### ThreadyMain

This test application creates a specified number of threads using the Thready sample class,
each with their own heartbeat, managed via the PaceMaker class.
It specifies "normal" and "fatal" heartbeat ranges of 500msec and 1 sec respectively.
A command-line option exists to vary the pulse rate in each thread randomly,
which will occasionally trigger events in the watchdog.

<!--
@startuml
class ThreadyMain
class Thready {
-string m_ProcessName
-string m_ThreadName
-bool m_Running
-bool m_Tamper
-bool m_Verbose
-PaceMaker m_PaceMaker
-milliseconds m_NormalLimit
-milliseconds m_AbsoluteLimit

+Thready( processName, id, normalLimit, absoluteLimit, tamper, verbose)
+void run()
+void quiesce()
}
class PaceMaker {
-string m_ProcessName
-string m_ThreadName
-string m_ActualName
-milliseconds m_NormalLimit
-milliseconds m_AbsoluteLimit
-mutex m_BeatMutex
-mapped_region m_Region

+PaceMaker( processName, threadName, normalLimit, absoluteLimit)
+void pulse()
}
class Heartbeat {
+milliseconds m_NormalLimit
+milliseconds m_AbsoluteLimit
+TTickCount m_Beat

+void SetCRC( heartbeat)
+bool isCRCOK( heartbeat)
+string makeActualName( processName, threadName)
+string extractUserName( actualName)
+string extractProcessName( actualName)
+string extractThreadName( actualName)
+pid_t extractProcessID( actualName)
+pid_t extractThreadID( actualName)
+bool isHeartbeat( name)
+utin32)t calcCRC( heartbeat)
+TTickCount tickCountNow()
}
ThreadyMain "1" *-- "n" Thready
Thready "1" *-- "1" PaceMaker
PaceMaker "1" *-- "1" Heartbeat
@enduml

-->

![](./ThreadyMain.svg)

### WatchdogMain

This test application uses the Watchdog class to manage heartbeats stored in shared memory.
The Watchdog class scans shared memory for heartbeats and monitors each one for timeliness.
It uses an instance of the EKG class to monitor a single heartbeat.

The WatchdogMain application has a global callback which will receive events for all applications.
In addition to this, the ThreadyMainWatchdogPolicy class also has a callback just for its events.

<!--
@startuml
class WatchdogMain

class Watchdog {
-bool m_Running
-bool m_AutoScan
-bool m_Verbose
-milliseconds m_ScanPeriod
-function m_Callback

+Watchdog( scanPeriod, autoScan, verbose)
+void setCallback( function)
+void setPolicy( policy)
+void monitor()
+void quiesce()

-void scanHeartbeats()
-void doCallbacks( actualName, processID, threadID, event, hbLength)
}

class WatchdogEvent {
+WatchdogEvent(actualName, processID, threadID, hbEvent, hbLength, info)
+string processName()
+string threadName()
+pid_t processID()
+pid_t threadID()
+HeartbeatEvent hbEvent()
+milliseconds hbLength()
+int info()
-string m_ProcessName
-string m_ThreadName
-pid_t m_ProcessID
-pid_t m_ThreadID
-HeartbeatEvent m_HBEvent
-milliseconds m_HBLength
-int m_Info
}

class IWatchdogPolicy {
#string m_ProcessName
+IWatchdogPolicy( processName)
+string processName()
+void handleEvent( event, verbose)
}

class ThreadyMainWatchdogPolicy {
+ThreadyMainWatchdogPolicy()
+void handleEvent( event, verbose)
}

class EKG {
-string m_ActualName
-string m_UserName
-pid_t m_ProcessID
-pid_t m_ThreadID
-mapped_region m_Region

+EKG( actualName)
+bool isAlive()
+bool isNormal()
+milliseconds length()
+string actualName()
+milliseconds normalLimit()
+pid_t processID()
+pid_t threadID()
}

class Heartbeat {
+milliseconds m_NormalLimit
+milliseconds m_AbsoluteLimit
+TTickCount m_Beat

+void SetCRC( heartbeat)
+bool isCRCOK( heartbeat)
+string makeActualName( processName, threadName)
+string extractProcessName( actualName)
+string extractThreadName( actualName)
+pid_t extractProcessID( actualName)
+pid_t extractThreadID( actualName)
+bool isHeartbeat( name)
+utin32)t calcCRC( heartbeat)
+TTickCount tickCountNow()
}
WatchdogMain "1" *-- "1" Watchdog
Watchdog "1" *-- "n" EKG
Watchdog "1" *-- "n" WatchdogEvent
EKG "1" *-- "1" Heartbeat
IWatchdogPolicy *-- WatchdogEvent
ThreadyMainWatchdogPolicy *-- WatchdogEvent
IWatchdogPolicy <|-- ThreadyMainWatchdogPolicy
WatchdogMain *-- ThreadyMainWatchdogPolicy
@enduml-->

![](./WatchdogMain.svg)

### How To Use

1. Build both applications with the provided cmake script.
2. In either order:
    1. In a terminal window, start ThreadyMain (verbose recommended).
    2. In a separate terminal window, start Watchdog (verbose recommended)
3. Examine shared memory (/dev/shm) to see ThreadyMain's heartbeats
4. If ThreadyMain is terminated, Watchdog should output events.
5. Examine both applications Usage for other options 