# --------------------------------------------------
# Foundation Libs
import FoundationPython as Foundation
import Entity
from common.singleton import *

# --------------------------------------------------
# Python Libs
import math


class EntityProjectile:
    def __init__(self):
        self.m_nSpeed = 0.0
        self.m_nPower = 0
        self.m_nPosition = Foundation.Vector3(0, 0, 0)
        self.m_nTarget = Foundation.Vector3(0, 0, 0)
        self.m_uEntityOwner = None

class EntityProjectileManager(Singleton):
    def __init__(self):
        self.m_uProjectileList = []

    def onUpdate(self, _nDeltaTime):
        for uProjectile in self.m_uProjectileList:
            pass

    def addProjectile(self, _uProjectile):
        self.m_uProjectileList.append(_uProjectile)


class EntityWeapon:
    def __init__(self):
        self.m_nPower = 0
        self.m_nRangeMax = 0.0
        self.m_nRangeMin = 0.0
        self.m_nCooldownTime = 0.0
        self.m_nProjectileSpeed = 0.0
        self.m_nFrontalArcAngle = math.pi * 2
        self.m_nCurrentCooldownTime = 0.0

    def setup(self, _nPower, _nRangeMin, _nRangeMax, _nCooldown, _nProjectileSpeed, _nFrontalArcAngle = math.pi * 2):
        self.m_nPower = _nPower
        self.m_nRangeMax = _nRangeMin
        self.m_nRangeMin = _nRangeMax
        self.m_nCooldownTime = _nCooldown
        self.m_nProjectileSpeed = _nProjectileSpeed
        self.m_nFrontalArcAngle = _nFrontalArcAngle

    def onUpdate(self, _nDeltaTime):
        self.m_nCurrentCooldownTime -= _nDeltaTime

    def fireAtEntity(self, _uTarget):
        if (self.m_nCurrentCooldownTime > 0.0):
            print "Cannot fire weapon yet", self.m_nCurrentCooldownTime, "remaining"
            return

        self.m_nCurrentCooldownTime = self.m_nCooldownTime


    def isWeaponRead(self):
        if (self.m_nCurrentCooldownTime > 0.0):
            return False
        return True
