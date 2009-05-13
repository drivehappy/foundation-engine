# --------------------------------------------------
# Python Libs
import stackless
from Queue import Queue
import random

# --------------------------------------------------
# Foundation Libs
import FoundationPython as Foundation
from Log.HTTPLogger import *
from Entity.Actor import Actor
from Entity.Properties import Property
from Entity.UnitPhysics import UnitPhysics
from Common.Common import Message, Team, WorldState, UnitState

# --------------------------------------------------
# Unit
class Unit(Actor):
    """
    Docstring.
    """

    def __init__(self, type = None, name = '', team = Team.UNKNOWN, position = Foundation.Vector3(0,20,0)):
        Actor.__init__(self, self.__handleTasklet)

        # Basic Attributes
        self.type = type
        self.name = name
        self.team = team
        self.physics = UnitPhysics()
        self.targetPosition = None
        self.graphic = None

        self.physics.setPosition(position)
        self.timer = Foundation.Timer()
        
        self.sphereData = Foundation.SphereData()
        self.sphereData.setRadius(self.type["Sight"])

        # Weapon
        self.weaponList = []

        # Unit Creation
        self.creationAbilities = []
        self.creationQueue = []
        self.creationType = []
        self.creationTimer = Foundation.Timer()
        self.creationTime = 0
        self.creatingUnit = False

        # Setup our creation abilities
        if type:
            self.creationAbilities = type["Creations"]
            self.physics.setVelocity(type["Speed"])

        # And finally, initialize our Unit graphics
        self.initGraphic()

        # SPHERE TREE DEMO
        self.RANDMOVETIME = 5.0

        # Add a non-blocking tasklet to quickly update our physics/graphics
        stackless.tasklet(self.__handleNonblockingTasklet)()
        stackless.schedule()

    def __handleTasklet(self, channelData):
        channel, msg, msgdata = channelData[0], channelData[1], channelData[2:]

        #print "Unit Update msg=", msg

        if msg == Message.WORLD_STATE:
            worldstate = msgdata[0]
            unitState = UnitState()
            #channel.send((self, Message.UNIT_STATE, unitState))

            # Do housekeeping tasks
            newUnitType = self.__updateUnitCreation(worldstate.deltaTime)
            if newUnitType:
                channel.send((self, Message.CREATE_UNIT, newUnitType, self.team, self.physics.getPosition()))

        elif msg == Message.UNIT_MOVE:
            targetPosition = msgdata[0]
            self.physics.moveTo(targetPosition)

        elif msg == Message.UNIT_SETTEAM:
            HTTPLogger().writeContent(LoggerError.SUCCESS, "Team set to %i" % (msgdata[0]))
            self.team = msgdata[0]

    def __handleNonblockingTasklet(self):
        while True:
            deltaTime = self.timer.getTime()
            self.timer.reset()
            deltaTime = Foundation.f_clamp(deltaTime, 0, 0.1)

            # RAND MOVE FOR SPHERE TREE
            self.RANDMOVETIME -= deltaTime
            if (self.RANDMOVETIME <= 0.0):
                self.RANDMOVETIME = 15.0
                targetPosition = Foundation.Vector3(random.randrange(-500, 500, 1), 20, random.randrange(-500, 500, 1))
                self.physics.moveTo(targetPosition)

            self.physics.doTask(deltaTime)
            self.graphic.setPosition(self.physics.getPosition())
            self.sphereData.setPosition(self.physics.getPosition())

            if self.shutdownFlag:
                raise TaskletExit
            stackless.schedule()


    # ------------------------------------------
    # Unit Creation
    def createUnit(self, type):
        if type:
            if (len(self.creationQueue) == 0):
                self.creationType = type
                self.creationTimer.reset()
                self.creatingUnit = True
                self.creationTime = type["BuildTime"]

            self.creationQueue.append(type)
            HTTPLogger().writeContent(LoggerError.NONE, "Entity %s queued unit for production." % (type["Name"]))
        else:
            HTTPLogger().writeContent(LoggerError.WARNING, "Attempted to create None type unit.")

    # Cancel the unit that matches in the back of the list
    def cancelUnitQueueInBack(self, _sUnit):
        # Run through our queue in reverse
        for nIndex, sUnit in enumerate(self.creationQueue.reverse()):
            if _sUnit == sUnit:
                self.creationQueue.remove(nIndex)

        # If we cancelled our 1 unit in creation progress...
        if (self.creationQueue.size() == 0):
            self.creationType = []
            self.creating = False
            self.creationTime = 0

    # Cancel the unit that matches in the front of the list, including unit in progress
    def cancelUnitQueueInFront(self, _sUnit):
        # Run through our queue in reverse
        bRemovedCreationUnit = False
        bRemovedUnit = False
        for nIndex, sUnit in enumerate(self.creationQueue):
            if _sUnit == sUnit["Name"]:
                del self.creationQueue[nIndex]
                bRemovedUnit = True

                if nIndex == 0:
                    # We removed the unit we're currently producing
                    bRemovedCreationUnit = True

                break

        # If we cancelled our 1 unit in creation progress...
        if bRemovedCreationUnit and (len(self.creationQueue) == 0):
            self.creationType = None
            self.creatingUnit = False
            self.creationTime = 0
        elif bRemovedCreationUnit and (len(self.creationQueue) > 0):
            # Set our creation type and shift the queue forward one
            self.creationType = self.creationQueue[0]
            self.creationTimer.reset()
            self.creatingUnit = True
            self.creationTime = self.creationType["BuildTime"]

        return bRemovedUnit

    #
    def getUnitQueue(self):
        return self.creationQueue
    
    # Check if we have a unit creation in progress
    #  called by the EntityManager so it can easily handle unit creation
    def __updateUnitCreation(self, _nDeltaTime):
        if (self.creatingUnit):
            if (self.creationTimer.getTime() >= self.creationTime):
                # Our unit in creation progress is done, pop it out and adjust the queue
                uUnitType = self.creationQueue[0]

                bUnitRemoved = self.cancelUnitQueueInFront(uUnitType["Name"])

                HTTPLogger().writeContent(LoggerError.NONE, "Removed unit from queue: " + uUnitType["Name"])

                # Return the type of unit to actually create
                return uUnitType
        return None

    def setPosition(self, _nPosition):
        self.physics.setPosition(_nPosition)

    # ------------------------------------------
    # Initialize Graphics
    def initGraphic(self):
        self.graphic = Foundation.EntityGraphic("SceneManager0", self.name + "_graphic")

        if self.type == None:
            HTTPLogger().writeContent(LoggerError.WARNING, "Entity type not set when attempting to setup graphics.")
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
        self.physics.setPosition(nPosition)
        self.graphic.setRotation(nRotation)
        self.graphic.setMaterial(sMaterial)
