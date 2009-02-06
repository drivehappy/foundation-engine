#if !defined(__F_TIMER_H_)
#define __F_TIMER_H_

#include "debug.h"
#include "memoryManager.h"

#if FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_LINUX
#   include <sys/time.h>
#   include <unistd.h>
#elif FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_WIN
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#endif

namespace Foundation {

    /** Class to provide an internal clock.
        @remarks
            API defines an OS independant representation of time.
    */
    class TimeManager {
    public:
        /** Get the singleton class.
        */
        static TimeManager& getSingleton();
        /** Return the engine time.
        */
        static double getTime();
        /** Allows the thread to sleep.
            @param
                Time in milliseconds to sleep
        */
        static void sleep(int milliseconds);
        /** Set the simulation factor
            @par
                Allows adjust of time.
        */
        static void setSimulationFactor(double _factor);

    protected:
        TimeManager();
        TimeManager(const TimeManager&);
        TimeManager& operator=(const TimeManager&);
        ~TimeManager();

    private:
#if FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_WIN
        static LARGE_INTEGER m_nStartTime;
#elif FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_LINUX
        static int timeval_subtract (struct timeval *result, struct timeval *x, struct timeval *y);
        static timeval m_nStartTime;
#else
        static double m_fStartTime;
#endif
        static double m_fSimulationFactor;

    };

    /** Class to provide a wrapper for singleton class TimeManager
        @remarks
            This is used to keep Python happy with our singleton class.
    */
    class TimeManager_PythonWrap {
    public:
        /** Default constructor
        */
        TimeManager_PythonWrap();

        /** Get the time from the real TimeManager
        */
        double getTime();

        /** Allows the thread to sleep
            @param
                Time in milliseconds.
        */
        void sleep(int milliseconds);

        /** Set the simulation factor
            @par
                Allows adjust of time.
        */
        void setSimulationFactor(double _factor);
    };

    /** Class to provide timer implementation.
        @par
            Interaces with the TimeManager to provide a stopwatch timer.
     */
    class Timer {
    public:
        Timer();
        Timer(const Timer&);
        //Timer& operator=(const Timer&);
        ~Timer();

        /** Pause the timer.
            @par
                Updates the running time and pauses the timer.
        */
        void pause();

        /** Resume the timer.
            @par
                Resumes the timer if it is paused.
        */
        void resume();

        /** Reset the timer.
            @par
                Resets the running time to 0 and pauses the timer.
        */
        void reset();

        /** Return the current timer value.
        */
        double getTime();

    private:
        double m_fStartTime;
        double m_fRunningTime;
        bool m_bPaused;
    };

};

#endif  // __F_TIMER_H_
