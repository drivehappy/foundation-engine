#if !defined(__F_DEFINES_H_)
#define __F_DEFINES_H_

#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include "platform.h"


namespace Foundation {

    typedef unsigned long uint64;
    typedef unsigned int uint32;
    typedef unsigned short uint16;
    typedef unsigned char uint8;
    
    #if FOUNDATION_ARCHITECTURE == FOUNDATION_ARCHITECTURE_64
    typedef uint64 s_size;
    typedef uint64* s_ptr;
    #else
    typedef uint32 s_size;
    typedef uint32* s_ptr;
    #endif
    
    #define FOUNDATION_VERSION_MAJOR 1
    #define FOUNDATION_VERSION_MINOR 0
    
    #define FOUNDATION_MAXSTRINGLEN 512
    #define FOUNDATION_CHUNKSIZE    (4096<<10)      /* 4mB */
    #if FOUNDATION_ARCHITECTURE == FOUNDATION_ARCHITECTURE_64
     #define FOUNDATION_MINBLOCKSIZE (8)             /* 8B - 64b */
    #else
     #define FOUNDATION_MINBLOCKSIZE (4)             /* 4B - 32b */
    #endif
    
    #define FOUNDATION_LIST_CAPACITY 32
    #define FOUNDATION_LIST_GROWSIZE 8
    
    #define USE_STL
    //#define USE_TINYXML
    #define USE_YAML

    //#define ENABLE_MEMORYMANAGER
};

#endif  // __F_DEFINES_H_
