# --------------------------------------------------
# Foundation Libs
import FoundationPython as Foundation
import Entity

# --------------------------------------------------
# Python Libs
import yaml


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

            uUnitType = uEntity.updateUnitCreation(_nDeltaTime)
            if (uUnitType != None):
                addEntity("RANDOM_NAME_HERE_FIXME", uUnitType)

    def addEntity(self, _sName, _sType):
        if not self.hasEntityType(_sType):
            print "[EntityManager] Error: EntityType " + _sType + " doesn't exist."
            return None
        else:
            entDerived = Entity.Entity(self.m_uManager.addEntity(_sName))
            entDerived.setType(self.EntityTypes[_sType])

            self.m_uEntityList.append(entDerived)

            return entDerived

    # This is used by the API for ingame unit creation
    def createEntity(self, _uConstructingEntity, _uType):
        if (_uConstructingEntity != None):
            _uConstructingEntity.createUnit(_uType)

    def getEntityTypes(self):
        return self.EntityTypes

    def hasEntityType(self, _sType):
        if self.EntityTypes:
            return _sType in self.EntityTypes
        return None

    def __readEntityTypesFromYaml(self, _sFilename):
        # Load entityTypes from a YAML file
        stream = file(_sFilename, 'r')
        self.EntityTypes = yaml.load(stream)['EntityTypes']


