# --------------------------------------------------
# Python Libs
import stackless
import yaml

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
            Actor.__init__(self)
            
            HTTPLogger().writeContent(LoggerError.NONE, "UnitManager Initialized.")
            
            self.entityTypes = None
            self.weaponTypes = None
            self.__readEntityTypesFromYaml(fileEntityTypes)
            self.__readWeaponTypesFromYaml(fileWeaponTypes)
            self.unitList = []

            stackless.tasklet(self.__runFrame)()

        # Tasklet
        def __handleTasklet(self, channelData):
            sender, msg, msgdata = channelData[0], channelData[1], channelData[2:]

            print "Manager Update"

            if msg == Message.CREATE_UNIT:
                HTTPLogger().writeContent(LoggerError.DEBUG, sender + " creating unit of type " + msgdata)
                self.addUnit(msgdata)

        # Tasklet
        def __sendWorldState(self):
            print "Sending WorldState"
            worldState = WorldState(1.0, Foundation.TimeManager().getTime())
            for unit in self.unitList:
                self.channel.send((self.channel, worldState))

        def __runFrame(self):
            while True:
                self.__sendWorldState()
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
                unitNew = Unit(uUnitType, "NewUnit")
                
                HTTPLogger().writeContent(LoggerError.NONE, "Created unit of type %s" % (uUnitType["Name"]))
                self.unitList.append(unitNew)

        # Unit API
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
