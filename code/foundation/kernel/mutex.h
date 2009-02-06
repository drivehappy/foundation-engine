#ifndef __F_MUTEX_H_
#define __F_MUTEX_H_

#include "platform.h"
#include "debug.h"

#if FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_LINUX
#   include <pthread.h>
#elif FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_WIN
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#endif

/**
 * Windows can use Mutexs and Critical Sections
 * from docs, Microsoft states critical sections are more
 * efficient in single process applications
 */
namespace Foundation {

    class Mutex {
    public:
        Mutex();
        ~Mutex();

        void Lock();
        void Unlock();

    private:
#if FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_WIN
        CRITICAL_SECTION m_hCriticalSection;
#elif FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_LINUX
        pthread_mutex_t m_pMutex;
#endif
    };


    inline Mutex::Mutex() {
#if FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_WIN
        InitializeCriticalSection(&m_hCriticalSection);
#elif FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_LINUX
        if (pthread_mutex_init(&m_pMutex, NULL) != 0)
            f_error("pthread_mutex_init failed");
#endif 
    }

    inline Mutex::~Mutex() {
#if FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_WIN
        DeleteCriticalSection(&m_hCriticalSection);
#elif FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_LINUX
        pthread_mutex_destroy(&m_pMutex);
#endif 
    }

    inline void Mutex::Lock() {
#if FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_WIN
        EnterCriticalSection(&m_hCriticalSection);
#elif FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_LINUX
        pthread_mutex_lock(&m_pMutex);
#endif  
    }

    inline void Mutex::Unlock() {
#if FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_WIN
        LeaveCriticalSection(&m_hCriticalSection);
#elif FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_LINUX
        pthread_mutex_unlock(&m_pMutex);
#endif  
    }

};

#endif  // __F_MUTEX_H_
