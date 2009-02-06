#include "entitySystem.h"

using namespace Foundation;
using namespace Foundation::Entities;

// Entity
Entity::Entity(EntityIdType _nId, const char *_sName)
{
	m_nId = _nId;
	
#if defined(FOUNDATION_DEBUG)
	if (strlen(_sName) > FOUNDATION_MAXSTRINGLEN) {
		f_printf("[Entity::Entity] Warning: Name exceeds length %i", FOUNDATION_MAXSTRINGLEN);
	}
#endif

	strcpy(m_sName, _sName);

    m_uGraphics = NULL;
}

unsigned int Entity::getId()
{
	return m_nId;
}

char* Entity::getName()
{
    char *sName = new char[FOUNDATION_MAXSTRINGLEN];
    strcpy(sName, m_sName);

    return sName;
}

void Entity::setName(const char *_sName)
{
    strcpy(m_sName, _sName);
}

void Entity::setGraphics(EntityGraphic *_pEntityGraphic)
{
    m_uGraphics = _pEntityGraphic;
}

EntityGraphic* Entity::getGraphics()
{
    return m_uGraphics;
}

// EntityManager
/// Static Data Member Vars
Task* EntityManager::m_pTask;

EntityManager::EntityManager()
{
	m_pTask = new Task(this, EntityManager::doTaskUpdate_Wrapper);
}

EntityManager::~EntityManager()
{
	// Cleanup entities
	vector<Entity *>::iterator	itrVec;

	for (itrVec = m_uEntityList.begin(); itrVec != m_uEntityList.end(); itrVec++) {
		delete (*itrVec);
	}
	m_uEntityList.clear();
}

EntityManager& EntityManager::getSingleton()
{
	static EntityManager m_uSingleton;
	return m_uSingleton;
}

void* EntityManager::doTaskUpdate(void *_pArgs)
{
	float nElapsedTime = *(float *)_pArgs;
    (void) nElapsedTime;

	return NULL;
}

Entity* EntityManager::addEntity(const char *_sName)
{
	EntityIdType nUniqueId = getAvailableUniqueId();
	Entity *pEntity = new Entity(nUniqueId, _sName);

	m_uEntityList.push_back(pEntity);

	return pEntity;
}

EntityIdType EntityManager::getAvailableUniqueId()
{
	vector<Entity *>::iterator	itrVec;
	EntityIdType				nId;
	bool						bUnique = false;
#if defined(FOUNDATION_DEBUG)
	unsigned int				nPass = 0;
	const unsigned int			nPassesWarning = 100;
#endif

	while (!bUnique) {
		nId = (unsigned int)rand();
		bUnique = true;

		for (itrVec = m_uEntityList.begin(); itrVec != m_uEntityList.end(); itrVec++) {
			if ((*itrVec)->getId() == nId) {
				bUnique = false;
				break;
			}
		}

#if defined(FOUNDATION_DEBUG)
		nPass++;
		if (nPass > nPassesWarning) {
			f_printf("[EntityManager::getAvailableUniqueId] Warning: Attempts to gather a unique ID exceeded warning size (%i / %i)", nPass, nPassesWarning);
		}
#endif
	}

	return nId;
}
