#if !defined(__F_PLATFORM_H_)
#define __F_PLATFORM_H_

namespace Foundation {
// Define the platform, compiler and architecture

#define FOUNDATION_PLATFORM_WIN     1
#define FOUNDATION_PLATFORM_LINUX   2
#define FOUNDATION_PLATFORM_MACOSX  3

#define FOUNDATION_COMPILER_MSVC    1

#define FOUNDATION_ARCHITECTURE_32  1
#define FOUNDATION_ARCHITECTURE_64  2

#if defined(WIN32)
#   define FOUNDATION_PLATFORM FOUNDATION_PLATFORM_WIN
#elif defined(LINUX)
#   define FOUNDATION_PLATFORM FOUNDATION_PLATFORM_LINUX
#elif defined(MACOSX)
#   define FOUNDATION_PLATFORM FOUNDATION_PLATFORM_MACOSX
#endif

#if defined(_DEBUG) || defined(DEBUG)
#   define FOUNDATION_DEBUG     1
#else
#   define FOUNDATION_RELEASE   1
#endif

#if defined(_MSC_VER)
#   define FOUNDATION_COMPILER          FOUNDATION_COMPILER_MSVC
#   define FOUNDATION_COMPILER_VERSION  _MSC_VER
#elif defined(__GNUC__)
#   define FOUNDATION_COMPILER          FOUNDATION_COMPILER_GNUC
#endif

#if defined(__x86_64__)
#   define FOUNDATION_ARCHITECTURE FOUNDATION_ARCHITECTURE_64
#else
#   define FOUNDATION_ARCHITECTURE FOUNDATION_ARCHITECTURE_32
#endif

#if FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_LINUX
#   include <sys/time.h>
#endif

};

#endif	// __F_PLATFORM_H_
