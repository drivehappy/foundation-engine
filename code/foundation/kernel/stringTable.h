#if !defined(__F_STRING_TABLE_H_)
#define __F_STRING_TABLE_H_

#include "memoryManager.h"

namespace Foundation {

    /** Class to provide hashes of strings.
        @par
            Utilizes the djb2 algorithm.
     */
    class StringTable {
    public:
        /** Get the singleton class.
         */
        static StringTable& getSingleton();

        /** Create a hash of the string.
            @param
                String to hash.
            @return
                Hashed string. 
         */
        static unsigned long getStringID(const char* _string);

    protected:
        StringTable();
        StringTable(const StringTable&);
        StringTable& operator=(const StringTable&);
        ~StringTable();
    };

};

#endif // __F_STRING_TABLE_H_

