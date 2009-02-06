#include "timer.h"
using namespace Foundation;

#if FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_WIN
LARGE_INTEGER TimeManager::m_nStartTime;
#elif FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_LINUX
timeval TimeManager::m_nStartTime;
#else
double TimeManager::m_fStartTime = 0;
#endif
double TimeManager::m_fSimulationFactor;

TimeManager::TimeManager()
{
#if FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_WIN
    QueryPerformanceCounter(&m_nStartTime);
#elif FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_LINUX
    gettimeofday(&m_nStartTime, NULL);
#endif

    m_fSimulationFactor = 1.0f;
}
TimeManager::~TimeManager()
{

}

TimeManager& TimeManager::getSingleton()
{
    static TimeManager m_pSingleton;
    return m_pSingleton;
}

double TimeManager::getTime()
{
    static double fTime;

#if FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_WIN
    static LARGE_INTEGER nCounter, nFreq;
    QueryPerformanceCounter(&nCounter);
    QueryPerformanceFrequency(&nFreq);

    fTime = double((nCounter.QuadPart - m_nStartTime.QuadPart) / (double)nFreq.QuadPart);
#elif FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_LINUX
    static timeval tv_diff, tv_now;
    gettimeofday(&tv_now, NULL);
    timeval_subtract(&tv_diff, &tv_now, &m_nStartTime);

    fTime = (tv_diff.tv_sec + (tv_diff.tv_usec / 1000000.0f));
#endif

    return fTime * m_fSimulationFactor;
}

#if FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_LINUX
/**
 * Taken from:
 * http://forums.devshed.com/c-programming-42/computing-time-elapsed-in-c-43197.html
 */
int TimeManager::timeval_subtract (struct timeval *result, struct timeval *x, struct timeval *y)
{
    /* Perform the carry for the later subtraction by updating y. */
    if (x->tv_usec < y->tv_usec) {
        int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
        y->tv_usec -= 1000000 * nsec;
        y->tv_sec += nsec;
    }
    if (x->tv_usec - y->tv_usec > 1000000) {
        int nsec = (x->tv_usec - y->tv_usec) / 1000000;
        y->tv_usec += 1000000 * nsec;
        y->tv_sec -= nsec;
    }

    /* Compute the time remaining to wait. tv_usec is certainly positive. */
    result->tv_sec = x->tv_sec - y->tv_sec;
    result->tv_usec = x->tv_usec - y->tv_usec;

    /* Return 1 if result is negative. */
    return x->tv_sec < y->tv_sec;
}
#endif

void TimeManager::sleep(int milliseconds)
{
#if FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_WIN
    Sleep(milliseconds);
#elif FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_LINUX
    usleep(milliseconds * 1000);
#else
#   error Add Me
#endif
}

void TimeManager::setSimulationFactor(double _factor) {
    m_fSimulationFactor = _factor;
}



TimeManager_PythonWrap::TimeManager_PythonWrap()
{
}

double TimeManager_PythonWrap::getTime()
{
    return TimeManager::getSingleton().getTime();
}

void TimeManager_PythonWrap::sleep(int milliseconds)
{
    TimeManager::getSingleton().sleep(milliseconds);
}

void TimeManager_PythonWrap::setSimulationFactor(double _factor)
{
    TimeManager::getSingleton().setSimulationFactor(_factor);
}


Timer::Timer()
{
    reset();
}
Timer::~Timer()
{
    m_fRunningTime = 0.0f;
    m_fStartTime = 0.0f;
}

void Timer::pause()
{
    if (!m_bPaused) {
        m_fRunningTime += TimeManager::getSingleton().getTime() - m_fStartTime;
        m_bPaused = true;
    } else {
#ifdef FOUNDATION_DEBUG
        //f_warn("Timer already paused.");
#endif
    }
}

void Timer::resume()
{
    if (m_bPaused) {
        m_fStartTime = TimeManager::getSingleton().getTime();
        m_bPaused = false;
    } else {
#ifdef FOUNDATION_DEBUG
        //f_warn("Timer wasn't already paused when trying to resume.");
#endif
    }
}

void Timer::reset()
{
    m_bPaused = false;
    m_fRunningTime = 0.0f;
    m_fStartTime = TimeManager::getSingleton().getTime();
}

double Timer::getTime()
{
    double fTime = m_fRunningTime;
    if (!m_bPaused)
        fTime += (TimeManager::getSingleton().getTime() - m_fStartTime);

    return fTime > 0.0f ? fTime : 0.0f;
}
