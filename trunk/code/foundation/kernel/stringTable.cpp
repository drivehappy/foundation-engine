#include "stringTable.h"

using namespace Foundation;

StringTable::StringTable() 
{
}

StringTable::~StringTable() 
{
}

StringTable& StringTable::getSingleton() 
{
    static StringTable m_pSingleton;
    return m_pSingleton;
}

unsigned long StringTable::getStringID(const char* _string) 
{
    /* djb2
     * This algorithm was first reported by Dan Bernstein
     * many years ago in comp.lang.c 
     */

    unsigned long hash = 5381;
    int c;

    while ((c = *_string++))
        hash = ((hash << 5) + hash) + c; // hash*33 + c

    return hash;
}
