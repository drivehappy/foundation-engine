# --------------------------------------------------
# Foundation Libs
import FoundationPython as Foundation

# --------------------------------------------------
# Python Libs
import math

# --------------------------------------------------
#
class EntityPhysics:
    
    def __init__(self):
        self.Task = Foundation.Task(self, "doTask")
        self.m_nPosition = Foundation.Vector3(0, 0, 0)
        self.m_nVelocity = 0
        self.m_nDirection = Foundation.Vector3(0, 0, 0)
        self.m_nTarget = Foundation.Vector3(0, 0, 0)
        self.m_bMoving = False

    def getForwardDirectionVector(self):
        return self.m_nVelocity.normalize()
    def getForwardDirectionAngle(self):
        return math.atan2(self.m_nVelocity[1], self.m_nVelocity[0])

    def getPosition(self):
        return self.m_nPosition
    def setPosition(self, _nPosition):
        self.m_nPosition = _nPosition

    def getVelocity(self):
        return self.m_nVelocity
    def setVelocity(self, _nVelocity):
        self.m_nVelocity = _nVelocity
        
    def getDirection(self):
        return self.m_nDirection
    def setDirection(self, _nDirection):
        self.m_nDirection = _nDirection

    def getMoving(self):
        return self.m_bMoving

    # --------------------------------------------------
    # Simple Behaviors
    def moveTo(self, _nTarget):
        self.m_nTarget = _nTarget
        self.m_nDirection = self.m_nTarget - self.m_nPosition
        self.m_nDirection.normalize()
        self.m_bMoving = True

    def stop(self):
        self.m_bMoving = False

    # --------------------------------------------------
    # Task
    def doTask(self, _nDeltaTime):
        if self.m_bMoving:
            # Simple radius check if we're close, set us on the target, if not proceed
            if (self.m_nPosition - self.m_nTarget).length() < 1.0:
                self.m_nPosition = self.m_nTarget
                self.stop()
            else:
                nDeltaPos = self.m_nVelocity * _nDeltaTime
                #nDeltaPosDirection = Foundation.Vector3(self.m_nDirection[0] * nDeltaPos, self.m_nDirection[1] * nDeltaPos, self.m_nDirection[2] * nDeltaPos)
                nDeltaPosDirection = self.m_nDirection * nDeltaPos
                self.m_nPosition += nDeltaPosDirection


            

