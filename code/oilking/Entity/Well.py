# --------------------------------------------------
# Python Libs
import stackless
import random

# --------------------------------------------------
# Foundation Libs
import FoundationPython as Foundation
from Entity.Actor import Actor

from Log.HTTPLogger import HTTPLogger, LoggerError

# --------------------------------------------------
# Well
class Well():

    def __init__(self, propertyOwner, name):
        self.name = name
        self.depth = random.randint(0, 5)
        self.structure = "None"
        self.propertyOwner = propertyOwner

    def getDrillCost(self):
        cost = (self.depth + 1) * 2000
        cost = Foundation.f_clamp(cost, 2000, 6000)
        
        return cost

    def getDrillCostMin(self):
        return 2000

    def getValue(self):
        if self.structure == "Derrick":
            return 1
        return 0

    def isTapped(self):
        if self.structure != "None":
            return True
        return False

    def drill(self):
        if self.depth >= 0 and self.depth <= 2:
            self.__createDerrick()
            HTTPLogger().writeContent(LoggerError.NONE, "Well drilled, depth was %i and created a Derrick." % (self.depth))
        else:
            self.__cap()
            HTTPLogger().writeContent(LoggerError.NONE, "Well drilled, depth was %i and capped." % (self.depth))

    def __createDerrick(self):
        if self.structure != "Cap":
            self.structure = "Derrick"
        else:
            HTTPLogger().writeContent(LoggerError.ERROR, "Attempted to build derrick on capped well.")

    def __cap(self):
        self.structure = "Cap"
