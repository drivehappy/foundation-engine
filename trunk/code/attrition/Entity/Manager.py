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

# --------------------------------------------------
# Class Manager
class Manager(Actor):
    """
    Docstring.
    """
    
    class __impl():
        def __init__(self, fileEntityTypes, fileWeaponTypes):
            HTTPLogger().writeContent(LoggerError.NONE, "UnitManager Initialized.")
            
            self.entityTypes = None
            self.weaponTypes = None
            self.__readEntityTypesFromYaml(fileEntityTypes)
            self.__readWeaponTypesFromYaml(fileWeaponTypes)
            self.unitList = []

        # Tasklet
        def __handleTasklet(self, channelData):
            for uEntity in self.m_uEntityList:
                uEntity.doTask(_nDeltaTime)

                # Check if the entity is done creating a unit
                uUnitType = uEntity.updateUnitCreation(_nDeltaTime)
                if (uUnitType != None):
                    uEntityNew = self.addEntity("RANDOM_NAME_HERE_FIXME", uUnitType)
                    uEntityNew.createGraphic("SceneManager0")
                    uEntityNew.createCollision(Foundation.CollisionShapeId.BOX, Foundation.Vector3(1, 1, 1))
                    uEntityNew.setPosition(Foundation.Vector3(30, 20, 30))
                    uEntityNew.moveTo(Foundation.Vector3(random.randrange(-500, 500, 1), 20, random.randrange(-500, 500, 1)))

                    HTTPLogger().writeContent(LoggerError.NONE, "Entity type %s created by %s" % (uUnitType["Name"], uEntity.getName()))


        # Retrieve type info from yaml files
        def __readEntityTypesFromYaml(self, filename):
            HTTPLogger().writeContent(LoggerError.NONE, "UnitManager loading entity types from %s" % (filename))

            # Load entityTypes from a YAML file
            stream = file(filename, 'r')
            self.EntityTypes = yaml.load(stream)['EntityTypes']

        def __readWeaponTypesFromYaml(self, filename):
            HTTPLogger().writeContent(LoggerError.NONE, "UnitManager loading weapon types from %s" % (filename))

            # Load entityTypes from a YAML file
            stream = file(filename, 'r')
            self.WeaponTypes = yaml.load(stream)['WeaponTypes']

    # Singleton
    __instance = None

    def __init__(self, fileEntityTypes, fileWeaponTypes):
        if Manager.__instance is None:
            Manager.__instance = Manager.__impl(fileEntityTypes, fileWeaponTypes)
        self.__dict__['_Manager__instance'] = Manager.__instance

    def __getattr__(self, attr):
        attr = getattr(self.__instance, attr)
        if (not attr):
            raise AttributeError
        else:
            return attr

    def __setattr__(self, attr, value):
        return setattr(self.__instance, attr, value)
