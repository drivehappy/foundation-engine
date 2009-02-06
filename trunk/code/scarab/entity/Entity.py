# --------------------------------------------------
# Foundation Libs
import FoundationPython as Foundation
from EntityCollision import *
from EntityPhysics import *

# --------------------------------------------------
#
class Entity(Foundation.Entity):

    def __init__(self, FoundationEntity):
        self.__bases__ = FoundationEntity
        self.Task = Foundation.Task(self, "doTask")
        self.Entity = FoundationEntity
        self.EntityGraphic = None
        self.m_uType = None
        self.EntityCollision = None
        self.EntityPhysics = EntityPhysics()

    def createGraphic(self, _sSceneManagerName):
        entityGraphics = Foundation.EntityGraphic(_sSceneManagerName, str(self.getId()) + "_GRAPHIC")

        self.setGraphics(entityGraphics)

        if self.m_uType == None:
            print "[Entity] Warning: Entity Type not set when attempting to setup graphics."
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

    def setPosition(self, _nPosition):
        self.m_nPosition = _nPosition
        self.EntityPhysics.setPosition(_nPosition)
        self.EntityGraphic.setPosition(_nPosition)

    def doTask(self, _nDeltaTime):
        self.EntityPhysics.doTask(_nDeltaTime)
        pass
        


