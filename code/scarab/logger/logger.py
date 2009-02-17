# --------------------------------------------------
# Foundation Libs
import FoundationPython as Foundation

FileManager = Foundation.FileManager()


class Logger:
    def __init__(self):
        pass

    def openLog(self, _sFilename):
        return FileManager.open(_sFilename, Foundation.FileAccess.APPEND, Foundation.FileMode.TEXT)

    def closeLog(self, _uFile):
        FileManager.close(_uFile)

    
