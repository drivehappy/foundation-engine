#include "py_fileSystem.hpp"

using namespace Foundation::FileSystem;

// File
File_PythonWrapper::File_PythonWrapper()
{
    // do nothing
}

str File_PythonWrapper::read(unsigned int _nElements)
{
    str sPythonString((const char *)File::read(_nElements));

    return sPythonString;
}

// FileManager
FileManager_PythonWrapper::FileManager_PythonWrapper()
{
}

File_PythonWrapper* FileManager_PythonWrapper::open(const char *_sFilename, FileAccess _uFileAccess, FileMode _uFileMode)
{
    return (File_PythonWrapper *)FileManager::getSingleton().open(_sFilename, _uFileAccess, _uFileMode);
}

void FileManager_PythonWrapper::close(File_PythonWrapper *_pFile)
{
    FileManager::getSingleton().close((File *)_pFile);
}

