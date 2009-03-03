# --------------------------------------------------
# Python Libs
import sys
import time

# --------------------------------------------------
# Foundation Libs
import FoundationPython as Foundation
from Logger import Logger

TimeManager = Foundation.TimeManager()

class LoggerError:
    NONE    = 0;
    WARNING = 1;
    ERROR   = 2;
    DEBUG   = 3;
    SUCCESS = 4;

# --------------------------------------------------
# Class HTTPLogger
class HTTPLogger:
    class __impl(Logger):
        def __init__(self, _sFilename):
            self.m_bLogOpened = False
            self.m_sFilename = _sFilename
            self.m_nRowCount = 0
            self.m_nLastWriteTime = 0.0

        def _checkOpen(self):
            if not self.m_bLogOpened:
                self.openLog(self.m_sFilename, Foundation.FileAccess.READ_WRITE_CREATE)
                self.m_bLogOpened = True

        def _get_callee_name(self, depth = 0):
            try:
                return sys._getframe(depth + 1).f_code.co_name
            except ValueError:
                return ""

        def _get_callee_stacknames(self):
            sMethodChain = ""
            sMethod = "."
            nMethodIndex = 2
            while len(sMethod) > 0:
                sMethod = self._get_callee_name(nMethodIndex)

                if len(sMethod) > 0 and sMethod != "<module>":
                    if nMethodIndex == 2:
                        sMethodChain += sMethod
                    else:
                        sMethodChain = sMethod + "." + sMethodChain

                nMethodIndex += 1

            return sMethodChain

        def newTable(self, _sTitle, *_sHeader):
            self._checkOpen()

            self.writeLog("<html>\n<head>\n<META HTTP-EQUIV='Content-Type' content='text/html; charset=utf-16'>\n</head>")
            self.writeLog("<pre><font face=arial size=+2>" + _sTitle + "</font></pre>\n")
            self.writeLog("<pre><font face=arial>Python " + sys.version + "</font></pre>\n")
            self.writeLog("<pre><font face=arial>Log Date: " + time.ctime() + "</font></pre>\n")
            self.writeLog("<table border=0 width=1000>\n<tr bgcolor=#7e7e7e>\n")
            self.writeLog("  <td><font face=arial color=#EEEEEE><b>Time(s)</b></font></td>\n")
            self.writeLog("  <td><font face=arial color=#EEEEEE><b>TimeDiff(s)</b></font></td>\n")
            self.writeLog("  <td><font face=arial color=#EEEEEE><b>Method</b></font></td>\n")
            for sHeader in _sHeader:
                self.writeLog("  <td><font face=arial color=#EEEEEE><b>" + str(sHeader) + "</b></font></td>\n")
            self.writeLog("</tr>\n")

        def writeContent(self, nErrorCode = LoggerError.NONE, *_sContent):
            nTimeDiff = self.m_nLastWriteTime
            self.m_nLastWriteTime = TimeManager.getTime()
            nTimeDiff = self.m_nLastWriteTime - nTimeDiff
            
            self._checkOpen()

            sBgColor = "#eeeeee"
            if self.m_nRowCount % 2:
                sBgColor = "#dedede"

            bBold = True
            if nErrorCode == LoggerError.NONE:
                sFontColor = "#3e3e3e"
                #bBold = False
            elif nErrorCode == LoggerError.WARNING:
                sFontColor = "orange"
            elif nErrorCode == LoggerError.ERROR:
                sFontColor = "red"
            elif nErrorCode == LoggerError.DEBUG:
                sFontColor = "blue"
            elif nErrorCode == LoggerError.SUCCESS:
                sFontColor = "green"

            sTime = "%6.6f" % (self.m_nLastWriteTime)
            sTimeDiff = "%6.6f" % (nTimeDiff)

            sMethodChain = self._get_callee_stacknames()
            # Truncate if it's too long
            if len(sMethodChain) > 30:
                sMethodChain = sMethodChain[:8] + "[...]" + sMethodChain[len(sMethodChain) - 18:]

            self.writeLog("<tr>\n")
            self.writeLog("  <td width=100 bgcolor=" + str(sBgColor) + "><font face=arial color=#3e3e3e>" + str(sTime) + "</font></td>\n")
            self.writeLog("  <td width=100 bgcolor=" + str(sBgColor) + "><font face=arial color=#3e3e3e>" + str(sTimeDiff) + "</font></td>\n")
            self.writeLog("  <td width=150 bgcolor=" + str(sBgColor) + "><font face=arial color=#3e3e3e>" + sMethodChain + "</font></td>\n")
            for sContent in _sContent:
                if bBold:
                    sContent = "<b>" + str(sContent) + "</b>"
                self.writeLog("  <td bgcolor=" + str(sBgColor) + "><font face=arial color=" + sFontColor + ">" + str(sContent) + "</font></td>\n")
            self.writeLog("</tr>\n")

            self.m_nRowCount += 1

        def endTable(self):
            self.writeLog("</table></center></html>")


    __instance = None

    def __init__(self, _sFilename = ""):
        if HTTPLogger.__instance is None:
            HTTPLogger.__instance = HTTPLogger.__impl(_sFilename)
        self.__dict__['_HTTPLogger__instance'] = HTTPLogger.__instance

    def __getattr__(self, attr):
        attr = getattr(self.__instance, attr)
        if (not attr):
            raise AttributeError
        else:
            return attr

    def __setattr__(self, attr, value):
        return setattr(self.__instance, attr, value)
