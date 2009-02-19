# --------------------------------------------------
# Foundation Libs
import FoundationPython as Foundation
from EntityCollision import *
from EntityPhysics import *
from EntityWeapon import *
from log.HTTPLogger import *

# --------------------------------------------------
#
class Entity(Foundation.Entity):

    def __init__(self, FoundationEntity):
        self.__bases__ = FoundationEntity
        self.Task = Foundation.Task(self, "doTask")
        self.Entity = FoundationEntity
        self.EntityGraphic = None
        self.EntityCollision = None
        self.EntityPhysics = EntityPhysics()

        self.m_uType = None

        # Unit Creation
        self.m_uCreationAbilityList = []
        self.m_uCreationQueueList = []
        self.m_uCreationTimer = Foundation.Timer()
        self.m_uCreationType = []
        self.m_nCreationTime = 0
        self.m_bCreating = False

        # Weapon List
        self.m_uWeaponList = []

    def createGraphic(self, _sSceneManagerName):
        entityGraphics = Foundation.EntityGraphic(_sSceneManagerName, str(self.getId()) + "_GRAPHIC")

        self.setGraphics(entityGraphics)

        if self.m_uType == None:
            HTTPLogger().writeContent(LoggerError.WARNING, "[Entity] Warning: Entity Type not set when attempting to setup graphics.")
        else:
            nScale      = self.m_uType["Scale"]
            nPosition   = self.m_uType["Position"]
            nRotation   = self.m_uType["Rotation"]
            sMesh       = self.m_uType["Mesh"]
            sMaterial   = self.m_uType["Material"]

            nScale = Foundation.Vector3(float(nScale[0]), float(nScale[1]), float(nScale[2]))
            nPosition = Foundation.Vector3(float(nPosition[0]), float(nPosition[1]), float(nPosition[2]))
            nRotation = Foundation.Quaternion(float(nRotation[0]), float(nRotation[1]), float(nRotation[2]), float(nRotation[3]))

            entityGraphics.setMesh(sMesh)
            entityGraphics.setScale(nScale)
            entityGraphics.setPosition(nPosition)
            entityGraphics.setRotation(nRotation)
            entityGraphics.setMaterial(sMaterial)

    def createCollision(self, _uShape, _nSize):
        self.EntityCollision = EntityCollision(_uShape, _nSize)

    def getId(self):
        sID = "ENTITYID_" + str(self.__bases__.getId())
        return sID

    def getName(self):
        return self.__bases__.getName()
    def setName(self, _sName):
        self.__bases__.setName(_sName)

    def getGraphics(self):
        return self.EntityGraphic
    def setGraphics(self, _uGraphics):
        self.EntityGraphic = _uGraphics

    def setType(self, _uType):
        self.m_uType = _uType
        self.EntityPhysics.setVelocity(self.m_uType["Speed"])
        self.setCreationAbilities(self.m_uType["Creations"])

    def setPosition(self, _nPosition):
        self.m_nPosition = _nPosition
        self.EntityCollision.setPosition(_nPosition)
        self.EntityPhysics.setPosition(_nPosition)
        self.EntityGraphic.setPosition(_nPosition)

    # ------------------------------------------
    # Creation Abilities
    def setCreationAbilities(self, _uUnitList):
        self.m_uCreationAbilityList = _uUnitList
    def appendCreationAbilities(self, _uUnitList):
        self.m_uCreationAbilityList.append(_uUnitList)
    def getCreationAbilities(self):
        return self.m_uCreationAbilityList

    # ------------------------------------------
    # Weapons
    def attachWeapon(self, _uWeapon):
        self.m_uWeaponList.append(_uWeapon)
    def detachWeapon(self, _uWeapon):
        del self.m_uWeaponList[_uWeapon]
    def getWeaponList(self):
        return self.m_uWeaponList

    # TODO: Obtain firing solution, figure out how to pass the requried
    #       entity direction back to this entity to fire upon the target

    # ------------------------------------------
    # Unit Creation
    def createUnit(self, _uUnit):
        if _uUnit:
            if (len(self.m_uCreationQueueList) == 0):
                self.m_uCreationType = _uUnit
                self.m_uCreationTimer.reset()
                self.m_bCreating = True
                self.m_nCreationTime = _uUnit["BuildTime"]

            self.m_uCreationQueueList.append(_uUnit)
            HTTPLogger().writeContent(LoggerError.NONE, "Entity %s queued unit for production." % (_uUnit["Name"]))
        else:
            HTTPLogger().writeContent(LoggerError.WARNING, "[Entity] Warning: Attempted to create None type unit.")

    # Cancel the unit that matches in the back of the list
    def cancelUnitQueueInBack(self, _sUnit):
        # Run through our queue in reverse
        for nIndex, sUnit in enumerate(self.m_uCreationQueueList.reverse()):
            if _sUnit == sUnit:
                self.m_uCreationQueueList.remove(nIndex)

        # If we cancelled our 1 unit in creation progress...
        if (self.m_uCreationQueueList.size() == 0):
            self.m_uCreationType = []
            self.m_bCreating = False
            self.m_nCreationTime = 0

    # Cancel the unit that matches in the front of the list, including unit in progress
    def cancelUnitQueueInFront(self, _sUnit):
        # Run through our queue in reverse
        bRemovedCreationUnit = False
        bRemovedUnit = False
        for nIndex, sUnit in enumerate(self.m_uCreationQueueList):
            if _sUnit == sUnit["Name"]:
                del self.m_uCreationQueueList[nIndex]
                bRemovedUnit = True
                
                if nIndex == 0:
                    # We removed the unit we're currently producing
                    bRemovedCreationUnit = True

                break

        # If we cancelled our 1 unit in creation progress...
        if bRemovedCreationUnit and (len(self.m_uCreationQueueList) == 0):
            self.m_uCreationType = ""
            self.m_bCreating = False
            self.m_nCreationTime = 0
        elif bRemovedCreationUnit and (len(self.m_uCreationQueueList) > 0):
            # Set our creation type and shift the queue forward one
            self.m_uCreationType = self.m_uCreationQueueList[0]
            self.m_uCreationTimer.reset()
            self.m_bCreating = True
            self.m_nCreationTime = self.m_uCreationType["BuildTime"]

        return bRemovedUnit

    #
    def getUnitQueue(self):
        return self.m_uCreationQueueList

    # Check if we have a unit creation in progress
    #  called by the EntityManager so it can easily handle unit creation
    def updateUnitCreation(self, _nDeltaTime):
        if (self.m_bCreating):
            if (self.m_uCreationTimer.getTime() >= self.m_nCreationTime):
                # Our unit in creation progress is done, pop it out and adjust the queue
                uUnitType = self.m_uCreationQueueList[0]

                bUnitRemoved = self.cancelUnitQueueInFront(uUnitType["Name"])

                HTTPLogger().writeContent(LoggerError.NONE, "[Entity] Removed unit from queue: " + uUnitType["Name"])
                
                # Inform the EntityManager to create the unit
                return uUnitType

    # ------------------------------------------
    #
    def moveTo(self, _nPosition):
        self.EntityPhysics.moveTo(_nPosition)
        HTTPLogger().writeContent(LoggerError.NONE, "Moving entity %s to: %f %f %f" % (self.getName(), _nPosition[0], _nPosition[1], _nPosition[2]))

    def attackUnit(self, _uUnit):
        if _uUnit:
            for uWeapon in self.m_uWeaponList:
                uWeapon.fireAtEntity(_uUnit)

    def doTask(self, _nDeltaTime):
        self.EntityPhysics.doTask(_nDeltaTime)

        self.setPosition(self.EntityPhysics.getPosition())

