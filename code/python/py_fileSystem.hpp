#if !defined(_PY__S_FILESYSTEM_HPP_)
#define _PY__S_FILESYSTEM_HPP_

// Foundation
#include "kernel/kernel.h"

// Boost
#include <boost/python.hpp>
using namespace boost::python;

namespace Foundation {

    namespace FileSystem {

        /**
        */
        class File_PythonWrapper : public File {
        public:
            File_PythonWrapper();

            str read(unsigned int _nElements);
        };

        /**
        */
        class FileManager_PythonWrapper : public FileManager {
        public:
            FileManager_PythonWrapper();

            File_PythonWrapper* open(const char *_sFilename, FileAccess _uFileAccess, FileMode _uFileMode);
            void close(File_PythonWrapper *_pFile);
        };

    };

};

#endif  // _PY__S_FILESYSTEM_HPP_

