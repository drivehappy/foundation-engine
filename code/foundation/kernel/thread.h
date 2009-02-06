#if !defined(__F_THREAD_H_)
#define __F_THREAD_H_

#include "memoryManager.h"
#include "timer.h"

#if FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_LINUX
#   include <pthread.h>
#   include <signal.h>
#endif

namespace Foundation {

    /** Class to provide OS independent threading.
     */
    class Thread {
    public:
        /** Thread constructor.
            @param
                Function pointer to the thread code.
            @param
                Pointer to a single argument.
            @param
                Pointer to a thread id.
         */
        Thread(void* (*func)(void*), void* args, uint32* nThreadID);

        /** Start the thread.
         */
        void start();

        /** Suspend the thread.
            @param
                Time in milliseconds to suspend.
         */
        void wait(uint32 nMilliseconds);

        /** Pause the thread.
         */
        void pause();

        /** Resume the thread.
         */
        void resume();

        /** Close the thread.
         */
        void close();

    private:
#if FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_WIN
        HANDLE          m_hThread;
#elif FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_LINUX
        pthread_t       m_hThread;
        pthread_cond_t  m_hCond;
        pthread_mutex_t m_hMutex;
        timespec        m_tAbsTime;
        bool            m_bPaused;
        void            *(*m_func)(void*);
        void            *m_args;
#endif
    };

};

#endif  // __F_THREAD_H_
