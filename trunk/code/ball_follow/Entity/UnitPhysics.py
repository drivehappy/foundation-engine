# --------------------------------------------------
# Python Libs
import stackless

# --------------------------------------------------
# Foundation Libs
import FoundationPython as Foundation
from Log.HTTPLogger import *

# --------------------------------------------------
# UnitPhysics
class UnitPhysics:
    def __init__(self, position=Foundation.Vector3(0,0,0)):
        self.position = position
        self.velocity = 0
        self.targetPosition = Foundation.Vector3(0,0,0)
        self.direction = Foundation.Vector3(0,0,0)

    def setPosition(self, position):
        self.position = position
    def getPosition(self):
        return self.position

    def setVelocity(self, velocity):
        self.velocity = velocity

    def moveTo(self, targetPosition):
        self.targetPosition = targetPosition
        self.direction = self.targetPosition - self.position
        self.direction.normalize()

    def doTask(self, _nDeltaTime):
        deltaPos = self.velocity * _nDeltaTime
        direction = Foundation.Vector3(self.direction[0] * deltaPos,
                                       0,
                                       self.direction[2] * deltaPos)
        self.position += Foundation.Vector3(direction[0], direction[1], direction[2])
