# --------------------------------------------------
# Foundation Libs
import FoundationPython as Foundation
import Entity

# --------------------------------------------------
# Python Libs
import math


# ---------------------------------
#
class EntityProjectile:
    def __init__(self):
        self.m_nSpeed = 0.0
        self.m_nPower = 0
        self.m_nPosition = Foundation.Vector3(0, 0, 0)
        self.m_nTarget = Foundation.Vector3(0, 0, 0)
        self.m_uEntityOwner = None
        
        # Projectile Graphics
        self.m_uGraphic = None

    def onUpdate(self, _nDeltaTime):
        self.m_nPosition += (self.m_nSpeed * _nDeltaTime)

    def setOwner(self, _uOwner):
        self.m_uEntityOwner = _uOwner

    def setPower(self, _nPower):
        self.m_nPower = _nPower

    def setSpeed(self, _nSpeed):
        self.m_nSpeed = _nSpeed

    def setPositionTarget(self, _nPosition, _nTarget):
        self.m_nTarget = _nTarget
        self.m_nPosition = _nPosition
        self.m_nDirection = self.m_nTarget - self.m_nPosition
        self.m_nDirection.normalize()

# -----------------------------------------------------
# Singleton Class to Handle Projectiles
class EntityProjectileManager(Singleton):
    class __impl:
        def __init__(self):
            self.m_uProjectileList = []

        def onUpdate(self, _nDeltaTime):
            for uProjectile in self.m_uProjectileList:
                uProjectile.onUpdate(_nDeltaTime)

        def addProjectile(self, _uProjectile):
            self.m_uProjectileList.append(_uProjectile)
        def getProjectileList(self):
            return self.m_uProjectileList

    __instance = None

    def __init__(self):
        if EntityProjectileManager.__instance is None:
            EntityProjectileManager.__instance = EntityProjectileManager.__impl()
        self.__dict__['_EntityProjectileManager__instance'] = EntityProjectileManager.__instance

    def __getattr__(self, attr):
        return getattr(self.__instance, attr)

    def __setattr__(self, attr, value):
        return setattr(self.__instance, attr, value)

# ---------------------------------
#
class EntityWeapon:
    def __init__(self):
        self.m_nPower = 0
        self.m_nSpeed = 0
        self.m_nRangeMax = 0.0
        self.m_nRangeMin = 0.0
        self.m_nCooldownTime = 0.0
        self.m_nProjectileSpeed = 0.0
        self.m_nFrontalArcAngle = math.pi * 2
        self.m_nCurrentCooldownTime = 0.0
        self.m_uTarget = None
        self.m_uOwner = None

    def setup(self, _nPower, _nSpeed, _nRangeMin, _nRangeMax, _nCooldown, _nProjectileSpeed, _nFrontalArcAngle = math.pi * 2):
        self.m_nPower = _nPower
        self.m_nSpeed = _nSpeed
        self.m_nRangeMax = _nRangeMin
        self.m_nRangeMin = _nRangeMax
        self.m_nCooldownTime = _nCooldown
        self.m_nProjectileSpeed = _nProjectileSpeed
        self.m_nFrontalArcAngle = _nFrontalArcAngle

    def onUpdate(self, _nDeltaTime):
        if (self.m_nCurrentCooldownTime > 0.0):
            self.m_nCurrentCooldownTime -= _nDeltaTime

        # Don't else this, it needs to be checked after the above OP
        if (self.m_nCurrentCooldownTime <= 0.0 and self.m_uTarget):
            fireAtEntity(self.m_uTarget)

    def fireAtEntity(self, _uTarget):
        self.m_uTarget = _uTarget

        if (self.m_nCurrentCooldownTime > 0.0):
            print "Cannot fire weapon yet", self.m_nCurrentCooldownTime, "remaining"
        else:
            self.m_nCurrentCooldownTime = self.m_nCooldownTime
            
            newProjectile = EntityProjectile()
            newProjectile.setOwner(self.m_uOwner)
            newProjectile.setPower(self.m_nPower)
            newProjectile.setSpeed(self.m_nSpeed)
            newProjectile.setPositionTarget(self.m_uOwner.getPosition(), self.m_uTarget.getPosition())
            EntityProjectileManager().addProjectile(newProjectile)

    def stopFiring(self):
        self.m_uTarget = None

    def isWeaponReady(self):
        if (self.m_nCurrentCooldownTime > 0.0):
            return False
        return True
