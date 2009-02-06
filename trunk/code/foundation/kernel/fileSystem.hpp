#if !defined(__F_FILESYSTEM_HPP_)
#define __F_FILESYSTEM_HPP_

// Standard
#include <stdio.h>

// STL
#include <vector>
using std::vector;

// Foundation
#include "stringTable.h"


namespace Foundation {

    namespace FileSystem {

        /**
        */
        enum FileAccess {
            READ = 0x0,
            WRITE,
            APPEND,
            READ_WRITE_EXISTS,
            READ_WRITE_CREATE,
            READ_APPEND,
        };

        /**
        */
        enum FileMode {
            BINARY = 0x0,
            TEXT,
        };

        /** File class for handling single files.
        */
        class File {
        public:
            /**
            */
            File();
            /**
            */
            ~File();

            /** Write to a valid file.
                @param
                    The handle to the file to write to.
                @param
                    The data to write.
            */
            void write(const char *_sData, unsigned int _nLength);

            /** Read from a valid file.
                @param
                    The handle to the file to write to.
                @param
                    The data to write.
            */
            char* read(unsigned int _nLength);
            /** Close the file.
            */
            unsigned int close();
            /** Set the internal File pointer.
            */
            void setFilePtr(FILE *_pFile);

        private:
            FILE            *m_pFILE;
            bool            m_bOpenState;
            vector<char *>  m_uReadData;
        };
    
        /** Singleton class for utilizing log dumps.
        */
        class FileManager {
        public:
            static FileManager& getSingleton();
    
            /** Open a new or existing file for reading/writing.
                @param
                    Filename to open or create.
                @return
                    Pointer to the file opened.
            */
            static File* open(const char *_sFilename, FileAccess _uFileAccess, FileMode _uFileMode);
            /** Close an opened file.
                @param
                    The file to close.
            */
            static void close(File *_pFile);

        protected:
            FileManager();
            ~FileManager();

        private:
            static vector<File *> m_uFileList;
        };

    };

};

#endif  // __F_FILESYSTEM_HPP_
