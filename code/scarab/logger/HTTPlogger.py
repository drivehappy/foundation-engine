# --------------------------------------------------
# Foundation Libs
import FoundationPython as Foundation
from logger import *

TimeManager = Foundation.TimeManager()

class LoggerError:
    NONE    = 0;
    WARNING = 1;
    ERROR   = 2;

class HTTPLogger(Logger):
    def __init__(self, _sFilename):
        self.m_bLogOpened = False
        self.m_sFilename = _sFilename
        self.m_nRowCount = 0

    def _checkOpen(self):
        if not self.m_bLogOpened:
            self.openLog(self.m_sFilename, Foundation.FileAccess.READ_WRITE_CREATE)
            self.m_bLogOpened = True


    def newTable(self, _sTitle, *_sHeader):
        self._checkOpen()

        self.writeLog("<center><font size='5'>" + _sTitle + "</font></center>\n")
        self.writeLog("<center><table border='0' width='800'>\n<tr bgcolor='#7e7e7e'>\n")
        for sHeader in _sHeader:
            self.writeLog("  <td><font color='#EEEEEE'>" + str(sHeader) + "</font></td>\n")
        self.writeLog("</tr>\n")

    def writeContent(self, nErrorCode, *_sContent):
        self._checkOpen()

        sBgColor = "#fefefe"
        if self.m_nRowCount % 2:
            sBgColor = "#dedede"

        sFontColor = "#3e3e3e"
        if nErrorCode == LoggerError.WARNING:
            sFontColor = "yellow"
        elif nErrorCode == LoggerError.ERROR:
            sFontColor = "red"

        sTime = "%6.4f" % TimeManager.getTime()

        self.writeLog("<tr>\n")
        self.writeLog("  <td bgcolor='" + str(sBgColor) + "'><font color='" + sFontColor + "'>" + str(sTime) + "</font></td>\n")
        for sContent in _sContent:
            self.writeLog("  <td bgcolor='" + str(sBgColor) + "'><font color='" + sFontColor + "'>" + str(sContent) + "</font></td>\n")
        self.writeLog("</tr>\n")

        self.m_nRowCount += 1

    def endTable(self):
        self.writeLog("</table></center>")

