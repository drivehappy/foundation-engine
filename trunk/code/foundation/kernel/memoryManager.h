#if !defined(__F_MEMORYMANAGER_)
#define __F_MEMORYMANAGER_

#include "defines.h"
#include "debug.h"

#ifdef USE_STL
#include <stdexcept>
#endif

#if FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_LINUX
#include <inttypes.h>
#endif


namespace Foundation {

    /** Struct to handle memory chunks
        @note
            Chunk of contiguous memory split into variable blocksizes.
            Use chunks <b>and</b> blocks to later optimize: cleanup any
            fragmentation inside chunks, skip over chunks determined
            to have no free room.
     */
    struct MemoryChunkHeader {
        struct MemoryChunkHeader *pNext;
        struct MemoryBlockHeader *pBlock;
        int nSize;
        int nBlockCount;
    };

    /** Struct to handle memory blocks
        @note
            Block of contiguous memory used by the memory manager
            allows us to save information on location/size and what
            source file/line issued an allocation for debugging.
     */
    struct MemoryBlockHeader {
        struct MemoryBlockHeader *pNext;
        int nSize;
        void *pMemory;
        bool bUsed;
#if defined(FOUNDATION_DEBUG)
        char cSrcName[FOUNDATION_MAXSTRINGLEN];
        int nSrcLine;
#endif
    };

    /** Struct to handle memory blocks
        @note
            Singleton Memory Manager to interface memory needs
            with the OS, to create faster malloc and free by
            pooling already allocated memory.
     */
    class MemoryManager {
    public:
        static MemoryManager& getSingleton();

        /** Check the allocated memory.
            @return
                True if a memory leak has been found; False if otherwise.
            @note
                Determine if we have any allocated blocks leftover
                and print out what allocated it.
        */
        static bool checkMemory();

        /** Dump our memory map to stdout.
            @note
                Dumps an entire output of allocated and unallocated chunks/blocks
                and the memory manager overhead.
        */
        static void dumpMap();

        /** Allocates memory directly from the OS.
            @param
                The size of memory in bytes.
            @param
                The name of the source file that allocated the chunk.
            @param
                The line of the source file that allocated the chunk.
            @return
                A pointer to the memory location.
        */
        static void* s_malloc(int nSize, const char* cSrcName, int nSrcLine);
        
        /** De-Allocates memory directly from the OS.
            @param
                The memory location.
        */
        static void s_free(void* pMemory);

    //protected:
        MemoryManager();
        MemoryManager(const MemoryManager&);
        MemoryManager& operator=(const MemoryManager&);
        ~MemoryManager();

    private:
        static MemoryChunkHeader *m_pChunkHead;

        /** Create a new chunk of memory.
            @param
                The size of memory in bytes.
            @param
                The name of the source file that allocated the chunk.
            @param
                The line of the source file that allocated the chunk.
            @return
                A pointer to a MemoryChunkHeader.
        */
        static MemoryChunkHeader* createChunk(int nSize, const char* cSrcName, int nSrcLine);

        /** Create a new block of memory.
            @param
                The "free" memory already allocated in a memory chunk.
            @param
                The size of memory in bytes.
            @param
                The name of the source file that allocated the chunk.
            @param
                The line of the source file that allocated the chunk.
            @return
                A pointer to a MemoryBlockHeader.
        */
        static MemoryBlockHeader* createBlock(void *pMemory, int nSize, const char* cSrcName, int nSrcLine);
    };

};


/**
 *  Operator overload for new and delete, this is layered on top of our memorymanager
 *  so it won't recursively call itself when dealing with internal memory
 */

using namespace Foundation;

#ifdef ENABLE_MEMORYMANAGER

    #if FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_WIN
    static
    #endif
    inline void* operator new(s_size nSize)
    #ifdef USE_STL
    throw (std::bad_alloc)
    #endif
    {
        void *p = MemoryManager::getSingleton().s_malloc(nSize, "Unk: new(s_size)", 0);
        f_assert(p);

        return p;
    }

    #if FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_WIN
    static
    #endif
    inline void* operator new(s_size nSize, const char* cSrcName, unsigned int iSrcLine)
    #ifdef USE_STL
    throw (std::bad_alloc)
    #endif
    {
        void *p = MemoryManager::getSingleton().s_malloc(nSize, cSrcName, iSrcLine);
        f_assert(p);

        return p;
    }

    #if FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_WIN
    static
    #endif
    inline void* operator new[](s_size nSize)
    #ifdef USE_STL
    throw (std::bad_alloc)
    #endif
    {
        void *p = MemoryManager::getSingleton().s_malloc(nSize, "Unk: new[](s_size)", 0);
        f_assert(p);

        return p;
    }

    #if FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_WIN
    static
    #endif
    inline void* operator new[](s_size nSize, const char* cSrcName, unsigned int iSrcLine)
    #ifdef USE_STL
    throw (std::bad_alloc)
    #endif
    {
        void *p = MemoryManager::getSingleton().s_malloc(nSize, cSrcName, iSrcLine);
        f_assert(p);

        return p;
    }

    #if FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_WIN
    static
    #endif
    inline void operator delete(void* pMemory)
    {
        MemoryManager::getSingleton().s_free(pMemory);
    }

    #if FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_WIN
    static
    #endif
    inline void operator delete(void* pMemory, const char* cSrcName, unsigned int nSrcLine)
    {
        MemoryManager::getSingleton().s_free(pMemory);
    }

    #if FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_WIN
    static
    #endif
    inline void operator delete[](void* pMemory)
    {
        MemoryManager::getSingleton().s_free(pMemory);
    }

    #if FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_WIN
    static
    #endif
    inline void operator delete[](void* pMemory, const char* cSrcName, unsigned int nSrcLine)
    {
        MemoryManager::getSingleton().s_free(pMemory);
    }

    // exp handles the Placement New
    #define new(exp) new(exp, __FILE__, __LINE__)

#endif  // _ENABLE_MEMORYMANAGER

#endif  // __F_MEMORYMANAGER_
