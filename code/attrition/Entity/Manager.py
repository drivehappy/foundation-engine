# --------------------------------------------------
# Python Libs
import stackless
import yaml
import random

# --------------------------------------------------
# Foundation Libs
import FoundationPython as Foundation
from Log.HTTPLogger import *
from Entity.Actor import Actor
from Entity.Properties import Property
from Entity.Unit import Unit
from Common.Common import Message, WorldState

# --------------------------------------------------
# Class Manager
class Manager():
    """
    Docstring.
    """
    
    class __impl(Actor):
        def __init__(self, fileEntityTypes, fileWeaponTypes):
            Actor.__init__(self, self.__handleTasklet)
            
            HTTPLogger().writeContent(LoggerError.NONE, "UnitManager Initialized.")

            self.deltaTime = Foundation.Timer()
            self.entityTypes = None
            self.weaponTypes = None
            self.__readEntityTypesFromYaml(fileEntityTypes)
            self.__readWeaponTypesFromYaml(fileWeaponTypes)
            self.unitList = []
            self.deltaTime = 0.0
            self.unitTypeCount = {}
            self.sphereTree = Foundation.SphereTree()

            stackless.tasklet(self.__runFrame)()

        # Tasklet
        def __handleTasklet(self, channelData):
            sender, msg, msgdata = channelData[0], channelData[1], channelData[2:]

            #print "Manager Update"

            if msg == Message.UNIT_STATE:
                #print "UnitState Message Received"
                pass
            elif msg == Message.CREATE_UNIT:
                unitType = msgdata[0]['Name']
                unitTeam = msgdata[1]
                HTTPLogger().writeContent(LoggerError.DEBUG, sender.name + " creating unit of type " + unitType)
                newUnit = self.addUnit(unitType)

                newUnit.channel.send((self.channel, Message.UNIT_SETTEAM, unitTeam))
                newUnit.channel.send((self.channel, Message.UNIT_MOVE, Foundation.Vector3(random.randrange(-500, 500, 1), 20, random.randrange(-500, 500, 1))))

        # Tasklet
        def __sendWorldState(self):
            worldState = WorldState(self.deltaTime, Foundation.TimeManager().getTime())
            
            for unit in self.unitList:
                unit.channel.send((self.channel, Message.WORLD_STATE, worldState))

        def __runFrame(self):
            while True:
                self.__sendWorldState()
                
                # Update the sphere tree, determine any data movements
                #self.sphereTree.update()
                #self.sphereTree.debugRender("SceneManager0");
                
                stackless.schedule()

        # Attempt a clean shutdown by killing all actors
        def shutdownUnits(self):
            for unit in self.unitList:
                unit.shutdown()
            self.shutdown()

        # addUnit
        def addUnit(self, unitType):
            uUnitType = self.getEntityTypeFromName(unitType)
            if not self.hasEntityType(unitType):
                HTTPLogger().writeContent(LoggerError.ERROR, "EntityType " + unitType + " doesn't exist.")
                return None
            else:
                typename = uUnitType['Name']
                if self.unitTypeCount.has_key(typename):
                    self.unitTypeCount[typename] += 1
                else:
                    self.unitTypeCount[typename] = 0

                unitNew = Unit(uUnitType, uUnitType['Name'] + str(self.unitTypeCount[typename]))

                HTTPLogger().writeContent(LoggerError.NONE, "Created unit of type %s" % (uUnitType["Name"]))
                self.unitList.append(unitNew)
                
                # Add the unit to the sphere tree structure
                self.sphereTree.addData(unitNew.sphereData)

                return unitNew
            return None

        # Unit API
        def getEntityByName(self, name):
            for entity in self.unitList:
                if entity.name == name:
                    return entity
            return None

        def getEntityTypes(self):
            return self.entityTypes

        def getEntityTypeFromName(self, name):
            if self.entityTypes:
                for unit in self.entityTypes:
                    if unit == name:
                        return self.entityTypes[unit]
            return None

        def hasEntityType(self, type):
            if self.entityTypes:
                return type in self.entityTypes
            return None

        # Retrieve type info from yaml files
        def __readEntityTypesFromYaml(self, filename):
            HTTPLogger().writeContent(LoggerError.NONE, "UnitManager loading entity types from %s" % (filename))

            # Load entityTypes from a YAML file
            stream = file(filename, 'r')
            self.entityTypes = yaml.load(stream)['EntityTypes']

        def __readWeaponTypesFromYaml(self, filename):
            HTTPLogger().writeContent(LoggerError.NONE, "UnitManager loading weapon types from %s" % (filename))

            # Load entityTypes from a YAML file
            stream = file(filename, 'r')
            self.weaponTypes = yaml.load(stream)['WeaponTypes']

    # Singleton
    __instance = None

    def __init__(self, fileEntityTypes, fileWeaponTypes):
        if Manager.__instance is None:
            Manager.__instance = Manager.__impl(fileEntityTypes, fileWeaponTypes)
        self.__dict__['_Manager__instance'] = Manager.__instance

    def __getattr__(self, attr):
        attr = getattr(self.__instance, attr)
        if (not attr):
            print "Attribute not found:", attr
            raise AttributeError
        else:
            return attr

    def __setattr__(self, attr, value):
        return setattr(self.__instance, attr, value)
