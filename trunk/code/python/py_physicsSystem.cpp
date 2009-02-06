#include "py_physicsSystem.h"

using namespace Foundation;
using namespace Foundation::Physics;

// Pickle BoatEngine
tuple boatengine_pickle_suite::getinitargs(const BoatEngine_PythonWrapper& boatEngine)
{
    return make_tuple(/*boatEngine.getPosition(), boatEngine.getRotation()*/);
}

tuple boatengine_pickle_suite::getstate(const BoatEngine_PythonWrapper& boatEngine)
{
    gmtlVector3_PythonWrapper nPosition = boatEngine.getPosition();
    gmtlQuaternion_PythonWrapper nRotation = boatEngine.getRotation();

    return make_tuple(nPosition, nRotation);
}

void boatengine_pickle_suite::setstate(BoatEngine_PythonWrapper& boatEngine, tuple state)
{
    if (len(state) != 2) {
        PyErr_SetObject(PyExc_ValueError,
                        ("expected 2-item tuple in call to __setstate__; got %s"
                        % state).ptr()
            );
        throw_error_already_set();
    }

    gmtl::Vec3f nPosition = gmtl::Vec3f(extract<gmtlVector3_PythonWrapper>(state[0]));
    gmtl::Quatf nRotation = gmtl::Quatf(extract<gmtlQuaternion_PythonWrapper>(state[1]));

    //f_printf("Debug Rotation: %f %f %f %f\n", nRotation[gmtl::Welt], nRotation[gmtl::Xelt], nRotation[gmtl::Yelt], nRotation[gmtl::Zelt]);
    //f_printf("Debug Position: %f %f %f\n", nPosition[gmtl::Xelt], nPosition[gmtl::Yelt], nPosition[gmtl::Zelt]);

    boatEngine.setPosition(nPosition);
    boatEngine.setRotation(nRotation);
}

// BoatEngine
gmtlVector3_PythonWrapper BoatEngine_PythonWrapper::getPosition() const
{
    return gmtlVector3_PythonWrapper(BoatEngine::getPosition());
}

gmtlQuaternion_PythonWrapper BoatEngine_PythonWrapper::getRotation() const
{
    return gmtlQuaternion_PythonWrapper(BoatEngine::getRotation());
}

void BoatEngine_PythonWrapper::setPosition(gmtlVector3_PythonWrapper _nPosition)
{
    BoatEngine::setPosition(_nPosition);
}

void BoatEngine_PythonWrapper::setRotation(gmtlQuaternion_PythonWrapper _nRotation)
{
    BoatEngine::setRotation(_nRotation);
}

gmtlVector3_PythonWrapper BoatEngine_PythonWrapper::getVelocity() const
{
    return (gmtlVector3_PythonWrapper)BoatEngine::getVelocity();
}

void BoatEngine_PythonWrapper::setVelocity(const gmtlVector3_PythonWrapper _nVelocity)
{
    BoatEngine::setVelocity(_nVelocity);
}

Task* BoatEngine_PythonWrapper::getTaskUpdate() const
{
    return BoatEngine::pTaskUpdate;
}

// RigidBody
void RigidBody_PythonWrapper::update(float fElapsedTime)
{
    return RigidBody::update(fElapsedTime);
}

void RigidBody_PythonWrapper::applyForce(const gmtlVector3_PythonWrapper _nForce)
{
    return RigidBody::applyForce(_nForce);
}

void RigidBody_PythonWrapper::applyAlignedForce(const gmtlVector3_PythonWrapper _nForce)
{
    return RigidBody::applyAlignedForce(_nForce);
}

void RigidBody_PythonWrapper::applyAngularForce(const gmtlVector3_PythonWrapper _nForce)
{
    return RigidBody::applyAngularForce(_nForce);
}

void RigidBody_PythonWrapper::applyAlignedAngularForce(const gmtlVector3_PythonWrapper _nForce)
{
    return RigidBody::applyAlignedAngularForce(_nForce);
}

gmtlVector3_PythonWrapper RigidBody_PythonWrapper::getPosition() const
{
    return gmtlVector3_PythonWrapper(RigidBody::getPosition());
}

gmtlQuaternion_PythonWrapper RigidBody_PythonWrapper::getRotation() const
{
    return gmtlQuaternion_PythonWrapper(RigidBody::getRotation());
}

// PhysicsManager
PhysicsManager_PythonWrap::PhysicsManager_PythonWrap()
{
}

PhysicsManager_PythonWrap::~PhysicsManager_PythonWrap()
{
}

Task *PhysicsManager_PythonWrap::getTaskUpdate() const {
    return PhysicsManager::getSingleton().pTaskUpdate;
}

void PhysicsManager_PythonWrap::initialize() {
    PhysicsManager::getSingleton().initialize();
};

void PhysicsManager_PythonWrap::destroy() {
    PhysicsManager::getSingleton().destroy();
};

RigidBody_PythonWrapper* PhysicsManager_PythonWrap::createRigidBody()
{
    RigidBody *_pBody = PhysicsManager::getSingleton().createRigidBody();
    RigidBody_PythonWrapper *_pBodyDerived = (RigidBody_PythonWrapper*)_pBody;

    return _pBodyDerived;
}

void PhysicsManager_PythonWrap::destroyRigidBody(RigidBody_PythonWrapper *_pBody)
{
    PhysicsManager::getSingleton().destroyRigidBody(_pBody);
}

unsigned int PhysicsManager_PythonWrap::createCollisionShape(CollisionShapeId _uShape, gmtlVector3_PythonWrapper _nSize)
{
    return PhysicsManager::getSingleton().createCollisionShape(_uShape, _nSize);
}

void PhysicsManager_PythonWrap::destroyCollisionObject(const unsigned int _nId)
{
    PhysicsManager::getSingleton().destroyCollisionObject(_nId);
}

void PhysicsManager_PythonWrap::setCollisionShapeOrigin(unsigned int _nId, gmtlVector3_PythonWrapper _nOrigin)
{
    PhysicsManager::getSingleton().setCollisionShapeOrigin(_nId, _nOrigin);
}

gmtlVector3_PythonWrapper PhysicsManager_PythonWrap::getCollisionShapeOrigin(unsigned int _nId)
{
    return PhysicsManager::getSingleton().getCollisionShapeOrigin(_nId);
}

void PhysicsManager_PythonWrap::setCollisionShapeRotation(unsigned int _nId, gmtlQuaternion_PythonWrapper _nRotation)
{
    PhysicsManager::getSingleton().setCollisionShapeRotation(_nId, _nRotation);
}

void PhysicsManager_PythonWrap::setCollisionShapeRotation(unsigned int _nId, gmtlVector3_PythonWrapper _nAxis, float _nAngle)
{
    PhysicsManager::getSingleton().setCollisionShapeRotation(_nId, _nAxis, _nAngle);
}

CollisionEvent_PythonWrapper* PhysicsManager_PythonWrap::consumeCollisionEvent()
{
    return (CollisionEvent_PythonWrapper*)PhysicsManager::getSingleton().consumeCollisionEvent();
}

unsigned int PhysicsManager_PythonWrap::getCollisionEventCount()
{
    return PhysicsManager::getSingleton().getCollisionEventCount();
}

void PhysicsManager_PythonWrap::destroyCollisionEvent(CollisionEvent_PythonWrapper *_pEvent)
{
    PhysicsManager::getSingleton().destroyCollisionEvent(_pEvent);
}

// CollisionEvent
unsigned int CollisionEvent_PythonWrapper::getBody0()
{
    return CollisionEvent::getBody0();
}

unsigned int CollisionEvent_PythonWrapper::getBody1()
{
    return CollisionEvent::getBody1();
}

gmtlVector3_PythonWrapper CollisionEvent_PythonWrapper::getContactPoint()
{
    return CollisionEvent::getContactPoint();
}

