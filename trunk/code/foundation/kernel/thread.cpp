#include "thread.h"

using namespace Foundation;


Thread::Thread(void* (*func)(void*), void* args, uint32* nThreadID)
{
#if FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_WIN
    m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)func, args, CREATE_SUSPENDED, (LPDWORD)nThreadID);
#elif FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_LINUX
    m_func = func;
    m_args = args;
#else
    f_warn("Threads not implemented on this platform.");
#endif
}

void Thread::start()
{
#if FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_WIN
    resume();
#elif FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_LINUX
    pthread_create(&m_hThread, NULL, m_func, m_args);
#endif
}

void Thread::wait(uint32 nMilliseconds)
{
#if FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_WIN
    WaitForSingleObject(m_hThread, nMilliseconds);
#elif FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_LINUX
    TimeManager::getSingleton().sleep(nMilliseconds);
    pthread_cancel(m_hThread);
#else
    f_warn("Thread::wait not implemented on this platform.");
#endif
}

void Thread::pause()
{
#if FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_WIN
    SuspendThread(m_hThread);
#elif FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_LINUX
    m_bPaused = true;
    pthread_mutex_lock(&m_hMutex);
    while (!m_bPaused)
        pthread_cond_wait(&m_hCond, &m_hMutex);
    pthread_mutex_unlock(&m_hMutex);
#else
    f_warn("Thread::pause not implemented on this platform.");
#endif
}

void Thread::resume()
{
#if FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_WIN
    ResumeThread(m_hThread);
#elif FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_LINUX
    m_bPaused = false;
#else
    f_warn("Thread::resume not implemented on this platform.");
#endif
}

void Thread::close()
{
#if FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_WIN
    CloseHandle(m_hThread);
#elif FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_LINUX
    pthread_exit(0);
#endif
}
