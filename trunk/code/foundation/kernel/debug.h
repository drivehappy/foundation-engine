#if !defined(__F_DEBUG_H_)
#define __F_DEBUG_H_

#include "defines.h"

#include <stdio.h>
#include <stdarg.h>

namespace Foundation {

    /**
     */
    struct CPUInfo {
        char sVendorName[16];
        unsigned int nProcessorInfo;
    };

    /** Class to provide debugging capabilities.
        @par
            Implements assertions, warnings and a printf wrapper.
     */
    class Debug {
    public:
        /** Get the singleton class.
         */
        static Debug& getSingleton();

        /** Function to print a variable number of args and format to the stdout.
            @param
                Format string to print.
            @param
                Variable args coinciding with the format string.
         */
        static void print(const char* cString, ...);

        /** Function to print a memory dump of specified location and size.
            @param
                Memory location.
            @param
                Size in bytes to display.
            @param
                Size of the memory type in bytes. (ie short = 2).
         */
        static void dumpMemory(void *pMemory, unsigned int nSize, unsigned int nTypeSize);

        /** Function to populate struct with CPU information.
         */
        static void getCPUInfo(CPUInfo & _uCPUInfo);

    protected:
        Debug();
        Debug(const Debug&);
        Debug& operator=(const Debug&);
        ~Debug();

    private:
        static char* m_cString;

    };

#if defined(FOUNDATION_DEBUG)
#   if FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_WIN
#       define f_assert(exp) { if(!(exp)) { Debug::getSingleton().print("Foundation Assertion:\n  %s\n  File: %s\n  Line: %i\n", #exp, __FILE__, __LINE__); __asm {int 3} } }
#   elif FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_LINUX
#       define f_assert(exp) { if(!(exp)) { Debug::getSingleton().print("Foundation Assertion:\n  %s\n  File: %s\n  Line: %i\n", #exp, __FILE__, __LINE__); /*asm ("int $0x03");*/ } }
#   endif

#   define f_error(exp) { if(!(exp)) { Debug::getSingleton().print("Foundation Error:\n  %s\n  File: %s\n  Line: %i\n", #exp, __FILE__, __LINE__); } }

#   define f_warn(exp) { Debug::getSingleton().print("Foundation Warning:\n  %s\n  File: %s\n  Line: %i\n", #exp, __FILE__, __LINE__); }

#   if FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_WIN
#       define f_breakpoint() { Debug::getSingleton().print("-- Internal breakpoint hit, check your debuggger.\n"); __asm {int 3} }
#   elif FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_LINUX
#       define f_breakpoint() { Debug::getSingleton().print("-- Internal breakpoint hit, check your debuggger.\n"); asm ("int $0x03"); }
#   endif

#else
# define f_assert(exp) { }
# define f_error(exp) { }
# define f_warn(exp) { }
#endif

/**
    Reference for how this f_printf works:
    http://publib.boulder.ibm.com/infocenter/lnxpcomp/v8v101/index.jsp?topic=/com.ibm.xlcpp8l.doc/language/ref/define.htm
 */
#if FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_WIN
#   define f_printf(exp, ...) { printf(exp, __VA_ARGS__); }
#elif FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_LINUX
#   define f_printf(...) { printf(__VA_ARGS__); }
#endif
};

#endif  // __F_DEBUG_H_
