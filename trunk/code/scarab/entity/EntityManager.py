# --------------------------------------------------
# Foundation Libs
import FoundationPython as Foundation
import Entity

# --------------------------------------------------
# Python Libs
import yaml
import random


class EntityManager(object):
    def __init__(self, _sEntityTypeFilename):
        self.m_uManager = Foundation.EntityManager();
        self.Task = Foundation.Task(self, "doTask")
        self.EntityTypes = None
        self.__readEntityTypesFromYaml(_sEntityTypeFilename)
        self.m_uEntityList = []

    def doTask(self, _nDeltaTime):
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

    def addEntity(self, _sName, _uType):
        if not self.hasEntityType(_uType["Name"]):
            print "[EntityManager] Error: EntityType " + _sType + " doesn't exist."
            return None
        else:
            entDerived = Entity.Entity(self.m_uManager.addEntity(_sName))
            entDerived.setType(_uType)

            self.m_uEntityList.append(entDerived)
            print "[EntityManager] Created Entity of Type %s" % (_uType["Name"])

            return entDerived

    # This is used by the API for ingame unit creation
    def createEntity(self, _uConstructingEntity, _uType):
        if (_uConstructingEntity != None):
            _uConstructingEntity.createUnit(_uType)

    def getEntityByName(self, _sName):
        for uEntity in self.m_uEntityList:
            if uEntity.getName() == _sName:
                return uEntity
        return None

    def getEntityById(self, _nId):
        for uEntity in self.m_uEntityList:
            if uEntity.getId() == _nId:
                return uEntity
        return None

    def getEntityTypes(self):
        return self.EntityTypes

    def getEntityTypeFromName(self, _sName):
        for sUnit in self.EntityTypes:
            if sUnit == _sName:
                return self.EntityTypes[sUnit]

    def hasEntityType(self, _sType):
        if self.EntityTypes:
            return _sType in self.EntityTypes
        return None

    def __readEntityTypesFromYaml(self, _sFilename):
        # Load entityTypes from a YAML file
        stream = file(_sFilename, 'r')
        self.EntityTypes = yaml.load(stream)['EntityTypes']


