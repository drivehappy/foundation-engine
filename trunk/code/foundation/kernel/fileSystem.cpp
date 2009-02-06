#include "fileSystem.hpp"

using namespace Foundation::FileSystem;

// Static data members
vector<File *> FileManager::m_uFileList;

// File
File::File()
{
    m_bOpenState = false;
    m_pFILE = NULL;
}

File::~File()
{
    close();
}

void File::write(const char *_sData, unsigned int _nLength)
{
    if (_nLength > 0) {
        if (m_bOpenState) {
            fwrite(_sData, sizeof(char), _nLength, m_pFILE);
            fflush(m_pFILE);
        } else {
            f_printf("[File] Error: Cannot write to file, file doesn't contain a reference.\n");
        }
    }
}

char* File::read(unsigned int _nLength)
{
    if (_nLength) {
        char *sNewData = new char[_nLength];

        if (m_pFILE) {
            fread(sNewData, 1, _nLength, m_pFILE);
        } else {
            f_printf("[File] Error: Cannot read from file, file doesn't contain a reference.\n");
        }

        m_uReadData.push_back(sNewData);

        return sNewData;
    } else {
        return NULL;
    }
}

unsigned int File::close()
{
    if (m_bOpenState) {
        vector<char *>::iterator itrData;
        for (itrData = m_uReadData.begin(); itrData != m_uReadData.end(); itrData++) {
            delete (*itrData);
        }
        m_bOpenState = false;

        return fclose(m_pFILE);
    } else {
        return 0;
    }
}

void File::setFilePtr(FILE *_pFile)
{
    m_pFILE = _pFile;
    m_bOpenState = true;
}

// FileSystem
FileManager& FileManager::getSingleton()
{
    static FileManager mSingleton;
    return mSingleton;
}

FileManager::FileManager()
{
}

FileManager::~FileManager()
{
}

File* FileManager::open(const char *_sFilename, FileAccess _uFileAccess, FileMode _uFileMode)
{
    char sFileMode[4] = { 0x0 };

    // Create File Access string
    switch (_uFileAccess) {
        case READ:
            strcat((char *)&sFileMode, "r");
            break;
        case WRITE:
            strcat((char *)&sFileMode, "w");
            break;
        case APPEND:
            strcat((char *)&sFileMode, "a");
            break;
        case READ_WRITE_EXISTS:
            strcat((char *)&sFileMode, "r+");
            break;
        case READ_WRITE_CREATE:
            strcat((char *)&sFileMode, "w+");
            break;
        case READ_APPEND:
            strcat((char *)&sFileMode, "a+");
            break;
        default:
            f_printf("[Foundation.FileManager] Warning: Unknown file access: 0x%x\n", _uFileAccess);
            break;
    }

    // Create File Mode string
    switch (_uFileMode) {
        case BINARY:
            strcat((char *)&sFileMode, "b");
            break;
        case TEXT:
            break;
        default:
            f_printf("[Foundation.FileManager] Warning: Unknown file mode: 0x%x\n", _uFileMode);
            break;
    }

    // Open the file
    FILE *pFile = fopen(_sFilename, sFileMode);
    if (!pFile) {
        f_printf("[Foundation.FileManager] Error: Could not open file %s\n", _sFilename);
        return NULL;
    }
    
    File *pOurFile = new File();
    pOurFile->setFilePtr(pFile);

    m_uFileList.push_back(pOurFile);
    
    return pOurFile;
}

void FileManager::close(File *_pFile)
{
    int nError = _pFile->close();

    vector<File *>::iterator itrVec;
    for (itrVec = m_uFileList.begin(); itrVec != m_uFileList.end(); itrVec++) {
        if ((*itrVec) == _pFile) {
            delete (*itrVec);
            m_uFileList.erase(itrVec);
            break;
        }
    }

    if (nError) {
        f_printf("[Foundation.FileManager] Error: Could not close file (%i)\n", nError);
        return;
    }
}
