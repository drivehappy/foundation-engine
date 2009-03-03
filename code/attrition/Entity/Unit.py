# --------------------------------------------------
# Python Libs
import stackless

# --------------------------------------------------
# Foundation Libs
import FoundationPython as Foundation
from Log.HTTPLogger import HTTPLogger
from Entity.Actor import Actor
from Entity.Properties import Property

# --------------------------------------------------
# Unit
class Unit(Actor):
    """
    Docstring.
    """

    def __init__(self, type = None, name = '', position = (0,0,0), velocity = (0,0,0)):
        self.type = type
        self.name = name
        self.position = position
        self.velocity = velocity
        self.targetPosition = None
        self.graphic = None

    def __handleTasklet(self, channelData):
        print "Unit.__handleTasklet:", channelData

    def initGraphic(self):
        self.graphic = Foundation.EntityGraphic("SceneManager0", self.name + "_graphic")

        if self.m_uType == None:
            HTTPLogger().writeContent(LoggerError.WARNING, "[Entity] Warning: Entity Type not set when attempting to setup graphics.")
            return

        nScale      = self.type["Scale"]
        nPosition   = self.type["Position"]
        nRotation   = self.type["Rotation"]
        sMesh       = self.type["Mesh"]
        sMaterial   = self.type["Material"]

        nScale = Foundation.Vector3(float(nScale[0]), float(nScale[1]), float(nScale[2]))
        nPosition = Foundation.Vector3(float(nPosition[0]), float(nPosition[1]), float(nPosition[2]))
        nRotation = Foundation.Quaternion(float(nRotation[0]), float(nRotation[1]), float(nRotation[2]), float(nRotation[3]))

        self.graphic.setMesh(sMesh)
        self.graphic.setScale(nScale)
        self.graphic.setPosition(nPosition)
        self.graphic.setRotation(nRotation)
        self.graphic.setMaterial(sMaterial)

    
