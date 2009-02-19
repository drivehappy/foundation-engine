# --------------------------------------------------
# Foundation Libs
import FoundationPython as Foundation

FileManager = Foundation.FileManager()


class Logger:
    def __init__(self):
        self.m_uFile = None

    def openLog(self, _sFilename, _uFileAccess):
        self.m_uFile = FileManager.open(_sFilename, _uFileAccess, Foundation.FileMode.TEXT)

    def closeLog(self):
        FileManager.close(self.m_uFile)

    def writeLog(self, _sString):
        self.m_uFile.write(_sString, len(_sString))
    
