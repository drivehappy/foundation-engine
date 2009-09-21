#include "py_entitySystem.h"

using namespace Foundation;
using namespace Foundation::Entities;

/*
Entity_PythonWrapper::Entity_PythonWrapper(EntityIdType _nId, const char *_sName)
	: Entity(_nId, _sName)
{
}
*/

EntityGraphic_PythonWrapper::EntityGraphic_PythonWrapper(const char *_sSceneManager, const char *_sMeshId)
    : EntityGraphic(_sSceneManager, _sMeshId)
{
}

void EntityGraphic_PythonWrapper::setScale(gmtlVector3_PythonWrapper _nScale)
{
    EntityGraphic::setScale(gmtl::Vec3f(_nScale));
}

void EntityGraphic_PythonWrapper::setRotation(gmtlQuaternion_PythonWrapper _nRotation)
{
    EntityGraphic::setRotation(gmtl::Quatf(_nRotation));
}

void EntityGraphic_PythonWrapper::setPosition(gmtlVector3_PythonWrapper _nPosition)
{
    EntityGraphic::setPosition(gmtl::Vec3f(_nPosition));
}

gmtlVector3_PythonWrapper EntityGraphic_PythonWrapper::getPosition()
{
    return EntityGraphic::getPosition();
}

void Entity_PythonWrapper::setGraphics(EntityGraphic_PythonWrapper *_pGraphic)
{
    Entity::setGraphics(_pGraphic);
}

EntityGraphic_PythonWrapper* Entity_PythonWrapper::getGraphics()
{
    return (EntityGraphic_PythonWrapper*)Entity::getGraphics();
}

PyObject* Entity_PythonWrapper::getName()
{
    char *pString = Entity::getName();

    return PyString_FromString(reinterpret_cast<char *>(pString));
}

EntityManager_PythonWrapper::EntityManager_PythonWrapper()
{
}

Entity_PythonWrapper* EntityManager_PythonWrapper::addEntity(const char *_sName)
{
	return (Entity_PythonWrapper *)EntityManager::getSingleton().addEntity(_sName);
}
