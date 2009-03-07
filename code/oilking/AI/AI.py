# --------------------------------------------------
# Python Libs
import stackless
import sys
import struct

# --------------------------------------------------
# Foundation Libs
import FoundationPython as Foundation
from Log.HTTPLogger import *

# --------------------------------------------------
# AIPlayer
class AIPlayer():

    def __init__(self, player=None):
        self.player = player

    def onTurn(self):
        # Attempt to drill on my property
        self.player.drillWell(self.properties[0].wells[0])

        derrickCount = self.player.getDerrickCount()
        HTTPLogger().writeContent(LoggerError.DEBUG, self.player.name + ": After drilling I now have %i Derricks" % (derrickCount))

        

