#include "debug.h"

using namespace Foundation;


char* Debug::m_cString = NULL;

Debug& Debug::getSingleton()
{
    static Debug m_pSingleton;
    return m_pSingleton;
}

Debug::Debug()
{
    m_cString = (char *)malloc(FOUNDATION_MAXSTRINGLEN *  sizeof(char));
}

Debug::~Debug()
{
    if (m_cString) {
        free(m_cString);
        m_cString = NULL;
    }
}

void Debug::print(const char* cString, ...)
{
    va_list arglist;
    va_start(arglist, cString);

#if FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_WIN
    _vsnprintf(m_cString, FOUNDATION_MAXSTRINGLEN, cString, arglist);
#elif FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_LINUX
    vsnprintf(m_cString, FOUNDATION_MAXSTRINGLEN, cString, arglist);
#elif FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_MACOSX
#   error Fixme
#endif
    printf(m_cString);

    va_end(arglist);
}

void Debug::dumpMemory(void *pMemory, unsigned int nSize, unsigned int nTypeSize)
{
    unsigned int nIndex, nIndexCol;
    const unsigned int nStepSize = 16;
    unsigned char nMemoryByte;

    f_printf("Memory Dump of %p (size = %i)\n", pMemory, nSize);
    for (nIndex = 0; nIndex < nSize; nIndex += nStepSize) {
        for (nIndexCol = 0; (nIndexCol < nStepSize) && ((nIndexCol + nIndex) < nSize); nIndexCol++) {
            nMemoryByte = *((unsigned char *)pMemory + nIndexCol + nIndex);

            if (nIndexCol == 0)
                f_printf("  %.2x | ", nIndex);
            f_printf("%.2x", nMemoryByte);
            if ( (nIndexCol == 0 && nTypeSize == 1) || ((nIndexCol+1) % nTypeSize == 0 && nIndexCol != 0) )
                f_printf(" ");
        }
        f_printf("\n");
    }
}

void Debug::getCPUInfo(CPUInfo & _uCPUInfo)
{
    // Determine if the cpuid instruction is supported
    //try {
#if FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_LINUX
        asm ("xor %eax, %eax");
        asm ("cpuid");
#elif FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_WIN
        __asm {
            xor eax, eax
            cpuid
        }
#else
        #error OS support has not been added.
#endif
    /*
    }
    catch () {
        f_printf("[Foundation.Debug] Error: Could not populate cpu info, cpuid instruction not supported.\n");
        return;
    }
    */

    // The cpuid instruction is available to our processor, continue
    memset(&_uCPUInfo.sVendorName, 0, 16);

#if FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_LINUX
    // Retrieve the Vendor Name
    asm ("xor %%eax, %%eax\n\t"
         "cpuid\n\t"
         "movl %%ebx, 0x0(%0)\n\t"
         "movl %%edx, 0x4(%0)\n\t"
         "movl %%ecx, 0x8(%0)"
        : /* no output */
        : "r"(_uCPUInfo.sVendorName)
        : "%eax", "%ebx", "%edx", "%ecx"
        );
    
    // Retrieve the CPU Model/Family/Stepping
    asm ("movl $0x1, %%eax\n\t"
         "cpuid\n\t"
         "movl %%eax, (%0)"
        : /* no output */
        : "r"(&_uCPUInfo.nProcessorInfo)
        : "%eax", "%ebx", "%edx", "%ecx"
        );

#elif FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_WIN
    // Retrieve the Vendor Name
    unsigned int nID[3];
    __asm {
        xor eax, eax
        cpuid
        mov nID+0, ebx
        mov nID+4, edx
        mov nID+8, ecx
    }
    memcpy(_uCPUInfo.sVendorName, nID, 12);

    // Retrieve the CPU Model/Family/Stepping
    __asm {
        mov eax, 1
        cpuid
        mov nID[0], eax
    }
    _uCPUInfo.nProcessorInfo = nID[0];
#else
    #error OS support has not been added.
#endif
}
