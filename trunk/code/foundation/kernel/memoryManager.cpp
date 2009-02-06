#include "memoryManager.h"

using namespace Foundation;


MemoryChunkHeader* MemoryManager::m_pChunkHead = NULL;

MemoryManager& MemoryManager::getSingleton()
{
    static MemoryManager m_pSingleton;
    return m_pSingleton;
}

MemoryManager::MemoryManager()
{
    m_pChunkHead = createChunk(FOUNDATION_CHUNKSIZE, __FILE__, __LINE__);
}

MemoryManager::~MemoryManager()
{
    MemoryChunkHeader *pWalkChunk = NULL;
    MemoryBlockHeader *pWalkBlock = NULL, *pWalkBlock2 = NULL;

    // For debugging leaks, prints out
    checkMemory();

    // Walk the chunks, and inner blocks - delete blocks for each, and then delete chunk
    while (m_pChunkHead) {
        pWalkChunk = m_pChunkHead;
        m_pChunkHead = m_pChunkHead->pNext;
        pWalkBlock = pWalkChunk->pBlock;
        while (pWalkBlock) {
            pWalkBlock2 = pWalkBlock;
            pWalkBlock = pWalkBlock->pNext;
            free(pWalkBlock2);
        }
        free(pWalkChunk);
    }

    m_pChunkHead = NULL;
}

bool MemoryManager::checkMemory()
{
    bool bLeak = false;

#if defined(FOUNDATION_DEBUG)
    MemoryChunkHeader *pWalkChunk = m_pChunkHead;
    MemoryBlockHeader *pWalkBlock = NULL;

    // Traverse our chunk list
    while (pWalkChunk) {
        // Walk through our blocks in our current chunk
        pWalkBlock = pWalkChunk->pBlock;
        while (pWalkBlock) {
            if (pWalkBlock->bUsed) {
                Debug::getSingleton().print("memleak: %p, size=%i, allocated at: %s:ln %i\n", pWalkBlock->pMemory, pWalkBlock->nSize, pWalkBlock->cSrcName, pWalkBlock->nSrcLine);
            }

            pWalkBlock = pWalkBlock->pNext;
        }
        pWalkChunk = pWalkChunk->pNext;
    }
#endif

    return bLeak;
}

MemoryChunkHeader* MemoryManager::createChunk(int nSize, const char* cSrcName, int nSrcLine)
{
    f_error(nSize);

    MemoryChunkHeader* pChunk = (MemoryChunkHeader*)malloc(sizeof(MemoryChunkHeader));
    f_assert(pChunk);

    pChunk->nBlockCount = 1;
    pChunk->nSize = nSize;
    pChunk->pNext = NULL;
    pChunk->pBlock = createBlock(malloc(nSize), nSize, cSrcName, nSrcLine);
    f_assert(pChunk->pBlock->pMemory);

    return pChunk;
}

MemoryBlockHeader* MemoryManager::createBlock(void *pMemory, int nSize, const char* cSrcName, int nSrcLine)
{
    //f_error(nSize && pMemory);

    if (nSize) {
        MemoryBlockHeader* pBlock = (MemoryBlockHeader*)malloc(sizeof(MemoryBlockHeader));
        f_assert(pBlock);
#if defined(FOUNDATION_DEBUG)
        strcpy(pBlock->cSrcName, cSrcName);
        pBlock->nSrcLine = nSrcLine;
#endif
        pBlock->nSize = nSize;
        pBlock->pNext = NULL;
        pBlock->pMemory = pMemory;
        pBlock->bUsed = false;

        return pBlock;
    } else
        return NULL;


}

void* MemoryManager::s_malloc(int nSize, const char* cSrcName, int nSrcLine)
{
    MemoryChunkHeader *pWalkChunk = m_pChunkHead, *pPrevChunk = NULL;
    MemoryBlockHeader *pWalkBlock = NULL;

    // Walk the chunks, and inner blocks
    while (pWalkChunk) {
        pWalkBlock = pWalkChunk->pBlock;
        while (pWalkBlock) {
            if (!pWalkBlock->bUsed && pWalkBlock->nSize >= nSize) {
                // Our requested size fits in this block, fill in our stuff
                // and create another at the end, tie them together

                if (!pWalkBlock->pNext) {
                    if (nSize < FOUNDATION_MINBLOCKSIZE)
                        nSize = FOUNDATION_MINBLOCKSIZE;
                    pWalkBlock->pNext = createBlock((void *)(intptr_t(pWalkBlock->pMemory) + nSize), pWalkBlock->nSize - nSize, cSrcName, nSrcLine);
                    pWalkChunk->nBlockCount++;
                }
                pWalkBlock->bUsed = true;
                pWalkBlock->nSize = nSize;
#if defined(FOUNDATION_DEBUG)
                pWalkBlock->nSrcLine = nSrcLine;
                strcpy(pWalkBlock->cSrcName, cSrcName);

                //f_printf("Allocating memory %p (%s:%i, size %i)\n", pWalkBlock->pMemory, pWalkBlock->cSrcName, pWalkBlock->nSrcLine, pWalkBlock->nSize);
#endif

                return pWalkBlock->pMemory;
            }
            pWalkBlock = pWalkBlock->pNext;
        }
        pPrevChunk = pWalkChunk;        // Use to link possible new chunk
        pWalkChunk = pWalkChunk->pNext;
    }

    // No space in our existing chunks/blocks, make a new one
    //f_warn(FOUNDATION_CHUNKSIZE > nSize);
    if (FOUNDATION_CHUNKSIZE < nSize) {
        pWalkChunk = createChunk(nSize, cSrcName, nSrcLine);

        f_error("CHECKME\n");
    } else {
        pWalkChunk = createChunk(FOUNDATION_CHUNKSIZE, cSrcName, nSrcLine);
        // We have room, create another unused chunk
        pWalkBlock = pWalkChunk->pBlock;
        pWalkBlock->bUsed = true;
        pWalkBlock->nSize = nSize;
        pWalkBlock->pNext = createBlock((void *)(intptr_t(pWalkBlock->pMemory) + nSize), FOUNDATION_CHUNKSIZE - pWalkBlock->nSize, cSrcName, nSrcLine);
        pWalkChunk->nBlockCount++;
#if defined(FOUNDATION_DEBUG)
        pWalkBlock->nSrcLine = nSrcLine;
        strcpy(pWalkBlock->cSrcName, cSrcName);

        //f_printf("Allocating memory %p (%s:%i, size %i)\n", pWalkBlock->pMemory, pWalkBlock->cSrcName, pWalkBlock->nSrcLine, pWalkBlock->nSize);
#endif
    }

    // Insure that the memoryManager is correctly setup, ie pPrevChunk != NULL
    if (pPrevChunk) {
        pPrevChunk->pNext = pWalkChunk;
        pWalkChunk->pBlock->bUsed = true;
    
        return pWalkChunk->pBlock->pMemory;
    } else {
        return NULL;
    }
}

void MemoryManager::s_free(void* pMemory)
{
    MemoryChunkHeader *pWalkChunk = m_pChunkHead;
    MemoryBlockHeader *pWalkBlock = NULL;

    // Find the block we need to free
    // Walk the chunks, and inner blocks - delete blocks for each, and then delete chunk
    while (pWalkChunk) {
        pWalkBlock = pWalkChunk->pBlock;
        while (pWalkBlock) {
            if (pWalkBlock->pMemory == pMemory && pWalkBlock->bUsed) {
#if defined(FOUNDATION_DEBUG)
                //f_printf("Freeing memory at %p (Allocated: %s:%i, Size: %i)\n", pWalkBlock->pMemory, pWalkBlock->cSrcName, pWalkBlock->nSrcLine, pWalkBlock->nSize);
                memset(pWalkBlock->pMemory, 0x0, pWalkBlock->nSize);
#endif
                pWalkBlock->bUsed = false;
                return;
#if defined(FOUNDATION_DEBUG)
            } else if (pWalkBlock->pMemory == pMemory && !pWalkBlock->bUsed) {
                //f_printf("Error: Re-Freeing unallocated memory at %p (Allocated: %s:%i, size: %i)\n", pWalkBlock->pMemory, pWalkBlock->cSrcName, pWalkBlock->nSrcLine, pWalkBlock->nSize);
                //f_breakpoint();
                return;
#endif
            }
            pWalkBlock = pWalkBlock->pNext;
        }
        pWalkChunk = pWalkChunk->pNext;
    }

#if defined(FOUNDATION_DEBUG)
    //f_printf("Error: Freeing unallocated memory at %p\n", pMemory);
    //dumpMap();
    //f_breakpoint();
#endif
}

void MemoryManager::dumpMap()
{
    MemoryChunkHeader *pWalkChunk = m_pChunkHead;
    MemoryBlockHeader *pWalkBlock = NULL;
    unsigned int i = 0, j = 0;

    // Debugging
#if defined(FOUNDATION_DEBUG)
    unsigned int iOverhead = 0;
    f_printf("\nMemory Map Dump:\n");
#endif

    // Walk the chunks, and inner blocks
    while (pWalkChunk) {
#if defined(FOUNDATION_DEBUG)
        iOverhead += (pWalkChunk->nBlockCount * sizeof(MemoryBlockHeader)) + sizeof(MemoryChunkHeader) + (FOUNDATION_MAXSTRINGLEN * pWalkChunk->nBlockCount);
        f_printf(" Chunk %d: Blocks: %d, MemoryStart: %p, Size: %d\n", i, pWalkChunk->nBlockCount, pWalkChunk->pBlock->pMemory, pWalkChunk->nSize);
#endif
        pWalkBlock = pWalkChunk->pBlock;
        while (pWalkBlock) {
#if defined(FOUNDATION_DEBUG)
            Debug::getSingleton().print("  Block %d: Used: %s, MemoryStart: %p, Size: %d\n     Allocation: %s:%d\n", j, pWalkBlock->bUsed ? "True" : "False", pWalkBlock->pMemory, pWalkBlock->nSize, pWalkBlock->cSrcName, pWalkBlock->nSrcLine);
#endif
            pWalkBlock = pWalkBlock->pNext;
            j++;
        }
        pWalkChunk = pWalkChunk->pNext;
        i++; j = 0;
    }

#if defined(FOUNDATION_DEBUG)
    f_printf("Manager Overhead: %d\n", iOverhead);
#endif
}
