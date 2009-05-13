#include "physicsSystem.h"

using namespace Foundation;
using namespace Foundation::Physics;

// Static data members
Task                                    *PhysicsManager::pTaskUpdate;
vector<PhysicsDebugObject*>             PhysicsManager::m_PhysicsDebugObjectList;
vector<RigidBody*>                      PhysicsManager::m_RigidBodyList;

btDefaultCollisionConfiguration         *PhysicsManager::collisionConfiguration;
btCollisionDispatcher                   *PhysicsManager::dispatcher;
//btAxisSweep3                            *PhysicsManager::broadphase;
btDbvtBroadphase                        *PhysicsManager::broadphase;
btCollisionWorld                        *PhysicsManager::collisionWorld;
map<unsigned int, btCollisionObject*>   PhysicsManager::collisionObjects;
vector<CollisionEvent *>                PhysicsManager::m_uCollisionEventList;
vector<btCollisionShape *>              PhysicsManager::m_uCollisionShapeList;
bool                                    PhysicsManager::m_bPaused;
// --

// CollisionEvent
CollisionEvent::CollisionEvent(btPersistentManifold *_uContactManifold)
{
    m_uContactManifold = new btPersistentManifold();
    memcpy(m_uContactManifold, _uContactManifold, sizeof(btPersistentManifold));
}

CollisionEvent::~CollisionEvent()
{
    delete m_uContactManifold;
}

unsigned int CollisionEvent::getBody0()
{
    btCollisionObject* pObject = static_cast<btCollisionObject*>(m_uContactManifold->getBody0());

    return PhysicsManager::getSingleton().getCollisionObjectId(pObject);
}

unsigned int CollisionEvent::getBody1()
{
    btCollisionObject* pObject = static_cast<btCollisionObject*>(m_uContactManifold->getBody1());

    return PhysicsManager::getSingleton().getCollisionObjectId(pObject);
}

gmtl::Vec3f CollisionEvent::getContactPoint()
{
    if (m_uContactManifold->getNumContacts() > 0) {
        btManifoldPoint nContactPoint = m_uContactManifold->getContactPoint(0);
        btVector3 nPoint = nContactPoint.getPositionWorldOnA();
        return gmtl::Vec3f(nPoint[0], nPoint[1], nPoint[2]);
    }

	return gmtl::Vec3f(0, 0, 0);
}

// RigidBody
RigidBody::RigidBody()
{
    m_nPosition = gmtl::Vec3f(0.0f, 0.0f, 0.0f);
    m_nVelocity = gmtl::Vec3f(0.0f, 0.0f, 0.0f);
    m_nForce = gmtl::Vec3f(0.0f, 0.0f, 0.0f);
    m_nRotation = gmtl::Quatf(0.0f, 0.0f, 0.0f, 1.0f);
    //m_nRotVelocity = gmtl::Quatf(0.0f, 0.0f, 0.0f, 1.0f);
    //m_nAngularVelocity = gmtl::Vec3f(0.0f, 0.0f, 0.0f);
    m_nAngularVelocity = 0.0f;
    m_nAngularForce = gmtl::Vec3f(0.0f, 0.0f, 0.0f);
    m_nAlignedAngularForce = gmtl::Vec3f(0.0f, 0.0f, 0.0f);
    m_nMass = 1.0f;
    m_nInertia = 1.0f;
    m_nFriction = 1.0f;
    m_nRotFriction = 1.0f;
}

void RigidBody::update(float nDeltaTime)
{
    gmtl::Vec3f nPosition       = m_nPosition;
    gmtl::Vec3f nVelocity       = m_nVelocity;
    gmtl::Vec3f nAcceleration   = gmtl::Vec3f(0, 0, 0);
    bool        bSetForce       = gmtl::length(m_nForce) > 0.0f;
    
    gmtl::Quatf q           = m_nRotation;
    //gmtl::Quatf dq          = m_nRotVelocity;
    gmtl::Vec3f ddq         = m_nAngularForce;
    bool        bSetTorque  = gmtl::length(ddq) > 0.0f;
    gmtl::Vec3f addq        = m_nAlignedAngularForce;
    bool        bSetAlignedTorque = gmtl::length(addq) > 0.0f;

    float       i           = m_nInertia;
    float       f           = m_nFriction;
    float       rf          = m_nRotFriction;

    //fElapsedTime = 0.005f;

    //f_printf("RidigBody Update: %f\n", fElapsedTime);
    //f_printf(" Rotation Length: %f\n", gmtl::length(q));
    //f_printf(" AlignedTorque Length: %f\n", gmtl::length(addq));

    // DEBUGGING - Reworking Simple Physics
    
    nAcceleration = m_nForce / m_nMass;
    nVelocity = m_nVelocity + (nAcceleration * nDeltaTime);
    nPosition = m_nPosition + (nVelocity * nDeltaTime);
    
    m_nPosition = nPosition;
    m_nVelocity = nVelocity;

    

    /*
    gmtl::Vec3f accel = (ddt / m) - (dt * f * fElapsedTime);    // acceleration is : (Force / Mass) - (Velocity * Friction * Time)
    dt += accel;                // add our acceleration to our Velocity
    t += dt * fElapsedTime;     // add our velocity * time to our position
    */

    /*
    gmtl::Vec3f accel = (ddt / m);                              // acceleration is : (Force / Mass) - (Velocity * Friction * Time)
    dt += accel;                                                // add our acceleration to our Velocity
    gmtl::Vec3f velAvg = (dt + m_nVelocity) * 0.5f;
    //velAvg *= f * fElapsedTime;
    t += velAvg * fElapsedTime;
    //dt -= velAvg;
    */

    /*
    dq += gmtl::Quatf(ddq[gmtl::Xelt], ddq[gmtl::Yelt], ddq[gmtl::Zelt], 0.0f);
    addq = (q * addq);        // Aligned Torque on rotation
    dq += gmtl::Quatf(addq[gmtl::Xelt], addq[gmtl::Yelt], addq[gmtl::Zelt], 0.0f);
    gmtl::Quatf rotVelDecay = dq * rf * fElapsedTime; // rotational velocity decreased by (RotVel * RotFriction * Time)
    dq -= rotVelDecay;
    q += (dq * q) / 2.0f * fElapsedTime;    // Rotation += (RotVel * Rotation) / 2.0 * Time
    gmtl::normalize(q);
    */
   
    /*
    ddq /= i;       // divide Torque by Inertia
    addq /= i;      // divide Rotation Torque by Inertia

    dq += gmtl::Quatf(ddq[gmtl::Xelt], ddq[gmtl::Yelt], ddq[gmtl::Zelt], 0.0f);
    addq = (q * addq);                                  // Aligned Torque on rotation
    dq += gmtl::Quatf(addq[gmtl::Xelt], addq[gmtl::Yelt], addq[gmtl::Zelt], 0.0f);
    gmtl::Quatf rotVelAvg = (dq + m_nRotVelocity) * 0.5f;
    gmtl::Quatf rotVelDecay = dq * rf * fElapsedTime;   // rotational velocity decreased by (RotVel * RotFriction * Time)
    dq -= rotVelDecay;
    q += rotVelAvg * fElapsedTime;                // Rotaion += (RotVel * Rotation) / 2.0 * Time
    gmtl::normalize(q);

    m_nPosition = t;
    m_nVelocity = dt;
    m_nRotation = q;
    m_nRotVelocity = dq;
    */

    // Optimize this, don't use a constructor, but set the vars
    if (bSetForce)
        m_nForce = gmtl::Vec3f(0,0,0);
    if (bSetTorque)
        m_nAngularForce = gmtl::Vec3f(0,0,0);
    if (bSetAlignedTorque)
        m_nAlignedAngularForce = gmtl::Vec3f(0,0,0);
}

void RigidBody::reset()
{
    m_nPosition = gmtl::Vec3f(0.0f, 0.0f, 0.0f);
    m_nVelocity = gmtl::Vec3f(0.0f, 0.0f, 0.0f);
    m_nForce = gmtl::Vec3f(0.0f, 0.0f, 0.0f);
    m_nRotation = gmtl::Quatf(0.0f, 0.0f, 0.0f, 1.0f);
    //m_nRotVelocity = gmtl::Quatf(0.0f, 0.0f, 0.0f, 1.0f);
    //m_nAngularVelocity = gmtl::Vec3f(0, 0, 0);
    m_nAngularVelocity = 0.0f;
    m_nAngularForce = gmtl::Vec3f(0.0f, 0.0f, 0.0f);
    m_nAlignedAngularForce = gmtl::Vec3f(0.0f, 0.0f, 0.0f);
    m_nMass = 1.0f;
    m_nInertia = 1.0f;
    m_nFriction = 1.0f;
    m_nRotFriction = 1.0f;
}

void RigidBody::applyForce(const gmtl::Vec3f _nForce)
{
    m_nForce += _nForce;
}

void RigidBody::applyAlignedForce(const gmtl::Vec3f _nForce)
{
    m_nForce += m_nRotation * _nForce;
}

void RigidBody::applyAngularForce(const gmtl::Vec3f _nForce)
{
    m_nAngularForce += _nForce;
}

void RigidBody::applyAlignedAngularForce(const gmtl::Vec3f _nForce)
{
    m_nAlignedAngularForce += m_nRotation * _nForce;
}

// PhysicsManager
PhysicsManager::PhysicsManager()
{
    pTaskUpdate = new Task(this, PhysicsManager::doTaskUpdate_Wrapper);

    // Init collision
    //btVector3 worldAabbMin(-10000,-10000,-10000);
    //btVector3 worldAabbMax(10000,10000,10000);
    //const int maxProxies = 1024;

    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    //dispatcher->registerCollisionCreateFunc(GIMPACT_SHAPE_PROXYTYPE, GIMPACT_SHAPE_PROXYTYPE, new btBoxBoxCollisionAlgorithm::CreateFunc);
    //btAxisSweep3 *pTest = new btAxisSweep3(worldAabbMin, worldAabbMax, maxProxies);
    broadphase = new btDbvtBroadphase();
    collisionWorld = new btCollisionWorld(dispatcher, broadphase, collisionConfiguration);
    //collisionWorld = new btCollisionWorld(dispatcher, pTest, collisionConfiguration);

    m_bPaused = false;
}

PhysicsManager::~PhysicsManager()
{
    destroy();
}

PhysicsManager& PhysicsManager::getSingleton()
{
    static PhysicsManager m_pSingleton;
    return m_pSingleton;
}

void* PhysicsManager::doTaskUpdate(void *_args)
{
    float fElapsedTime = *(float*)_args;
    vector<RigidBody*>::iterator itrVec;

    if (!m_bPaused) {
        // Work on collisions
        if (collisionWorld) {
            collisionWorld->updateAabbs();
            collisionWorld->getBroadphase()->calculateOverlappingPairs(dispatcher);
            dispatcher->dispatchAllCollisionPairs(broadphase->getOverlappingPairCache(), collisionWorld->getDispatchInfo(), dispatcher);
            collisionWorld->performDiscreteCollisionDetection();
            
            int numManifolds = collisionWorld->getDispatcher()->getNumManifolds();

            for (int i=0; i<numManifolds; i++) {
                btPersistentManifold *contactManifold = collisionWorld->getDispatcher()->getManifoldByIndexInternal(i);
                CollisionEvent *pNewCollision = new CollisionEvent(contactManifold);

                m_uCollisionEventList.push_back(pNewCollision);
            }
        }
    }

    return NULL;
}

void PhysicsManager::initialize()
{
}

void PhysicsManager::destroy()
{
    vector<RigidBody*>::iterator itrVec;
    vector<btCollisionShape*>::iterator itrCollisionShape;

    // Cleanup any leftover rigid bodies in our system
    for (itrVec = m_RigidBodyList.begin(); itrVec != m_RigidBodyList.end(); itrVec++) {
        delete (*itrVec);
    }
    m_RigidBodyList.clear();

    // Cleanup any leftover collisionshapes
    for (itrCollisionShape = m_uCollisionShapeList.begin(); itrCollisionShape != m_uCollisionShapeList.end(); itrCollisionShape++) {
        delete (*itrCollisionShape);
    }
    m_uCollisionShapeList.clear();
}

RigidBody* PhysicsManager::createRigidBody()
{
    RigidBody *pRigidBody = new RigidBody();

    m_RigidBodyList.push_back(pRigidBody);

    return pRigidBody;
}

void PhysicsManager::destroyRigidBody(RigidBody *_pBody)
{
    vector<RigidBody*>::iterator itrVec;
    
    f_assert(_pBody);

    for (itrVec = m_RigidBodyList.begin(); itrVec != m_RigidBodyList.end(); itrVec++) {
        if ((*itrVec) == _pBody) {
            delete (*itrVec);
            m_RigidBodyList.erase(itrVec);
            break;
        }
    }
}

unsigned int PhysicsManager::createCollisionShape(CollisionShapeId _uShape, gmtl::Vec3f _nSize)
{
    btCollisionShape *pCollisionShape = NULL;
    btCollisionObject *pCollisionObject = new btCollisionObject();
    vector<btCollisionShape *>::iterator itrVec;
    btMatrix3x3 uBasis;
    btVector3   nSize = btVector3(_nSize[gmtl::Xelt], _nSize[gmtl::Yelt], _nSize[gmtl::Zelt]);

    // Setup the collision shape
    if (_uShape == BOX) {
        // Search through our list, find any existing box shapes
        for (itrVec = m_uCollisionShapeList.begin(); itrVec != m_uCollisionShapeList.end(); itrVec++) {
            btVector3 nShapeSize = ((btBoxShape*)(*itrVec))->getHalfExtentsWithMargin();
            if (nShapeSize.x() == nSize.x() && nShapeSize.y() == nSize.y() && nShapeSize.z() == nSize.z()) {
                pCollisionShape = (*itrVec);
                break;
            }
        }

        if (!pCollisionShape) {
            pCollisionShape = new btBoxShape(nSize);
            m_uCollisionShapeList.push_back(pCollisionShape);
        }
    } else if (_uShape == SPHERE) {
        // Search through our list, find any existing sphere shapes
        for (itrVec = m_uCollisionShapeList.begin(); itrVec != m_uCollisionShapeList.end(); itrVec++) {
            btScalar nShapeSize = ((btSphereShape*)(*itrVec))->getRadius();
            if (nShapeSize == nSize.length()) {
                pCollisionShape = (*itrVec);
                break;
            }
        }

        if (!pCollisionShape) {
            pCollisionShape = new btSphereShape((btScalar)nSize.length());
            m_uCollisionShapeList.push_back(pCollisionShape);
        }
    }

    if (!pCollisionShape) {
        f_printf("[PhysicsManager] Error: Could not find or create collision shape object of shapeId %i\n", _uShape);
        return 0;
    }

    // Setup the collision object
    uBasis.setIdentity();
    pCollisionObject->getWorldTransform().setBasis(uBasis);
    pCollisionObject->getWorldTransform().setIdentity();
    pCollisionObject->getWorldTransform().setOrigin(btVector3(0, 0, 0));
    pCollisionObject->setCollisionShape(pCollisionShape);
    collisionWorld->addCollisionObject(pCollisionObject);
    collisionObjects[(unsigned int)pCollisionObject] = pCollisionObject;

    return (unsigned int)pCollisionObject;
}

void PhysicsManager::destroyCollisionObject(const unsigned int _nId)
{
    btCollisionObject *pObject = collisionObjects[_nId];

    if (pObject) {
        collisionWorld->removeCollisionObject(pObject);
    } else {
        f_printf("[PhysicsManager.destroyCollisionObject] Error: Cannot find object of id: %i.\n", _nId);
    }
}

void PhysicsManager::setCollisionShapeOrigin(unsigned int _nId, gmtl::Vec3f _nOrigin)
{
    btCollisionObject *pObject = getCollisionObject(_nId);

    //f_printf("Setting object origin to: %f, %f, %f\n", newOrigin.x(), newOrigin.y(), newOrigin.z());
    if (pObject) {
        btVector3 newOrigin = btVector3(_nOrigin[gmtl::Xelt], _nOrigin[gmtl::Yelt], _nOrigin[gmtl::Zelt]);
        pObject->getWorldTransform().setOrigin(newOrigin);
    } else {
        f_printf("[PhysicsManager] Error: Could not find collision object of id %i\n", _nId);
    }
}

gmtl::Vec3f PhysicsManager::getCollisionShapeOrigin(unsigned int _nId)
{
    btCollisionObject *pObject = getCollisionObject(_nId);

    if (pObject) {
        btVector3 nOrigin = pObject->getWorldTransform().getOrigin();
        return gmtl::Vec3f(nOrigin[0], nOrigin[1], nOrigin[2]);
    } else {
        f_printf("[PhysicsManager] Error: Could not find collision object of id %i\n", _nId);
    }

	return gmtl::Vec3f(0, 0, 0);
}

void PhysicsManager::setCollisionShapeRotation(unsigned int _nId, gmtl::Quatf _nRotation)
{
    btCollisionObject *pObject = getCollisionObject(_nId);
    btQuaternion newRotation = btQuaternion(_nRotation[gmtl::Xelt], _nRotation[gmtl::Yelt], _nRotation[gmtl::Zelt], _nRotation[gmtl::Welt]);

    if (pObject) {
        pObject->getWorldTransform().setRotation(newRotation);
    } else {
        f_printf("[PhysicsManager] Error: Could not find collision object of id %i\n", _nId);
    }
}

void PhysicsManager::setCollisionShapeRotation(unsigned int _nId, gmtl::Vec3f _nAxis, float _nAngle)
{
    btCollisionObject *pObject = getCollisionObject(_nId);
    btQuaternion newRotation(0, 0, 0, 1);

    if (pObject) {
        btVector3 nAxis = btVector3(_nAxis[gmtl::Xelt], _nAxis[gmtl::Yelt], _nAxis[gmtl::Zelt]);
        newRotation.setRotation(nAxis, (btScalar)_nAngle);
        //f_printf("Shape Rotation: %f, %f, %f, %f\n", newRotation[0], newRotation[1], newRotation[2], newRotation[3]);
        pObject->getWorldTransform().setRotation(newRotation);
    } else {
        f_printf("[PhysicsManager] Error: Could not find collision object of id %i\n", _nId);
    }
}

unsigned int PhysicsManager::getCollisionObjectId(btCollisionObject *_pObject)
{
    map<unsigned int, btCollisionObject*>::iterator itrMap;
    unsigned int nId = 0;

    for (itrMap = collisionObjects.begin(); itrMap != collisionObjects.end(); itrMap++) {
        if ((*itrMap).second == _pObject) {
            nId = (*itrMap).first;
            break;
        }
    }

    if (nId == 0) {
        f_printf("[Foundation.PhysicsManager] Error: Could not retrieve the ID for collision object %p\n", _pObject);
    }

    return nId;
}

btCollisionObject* PhysicsManager::getCollisionObject(unsigned int _nId)
{
    return collisionObjects[_nId];
}

CollisionEvent* PhysicsManager::consumeCollisionEvent()
{
    CollisionEvent *pObject = NULL;
    vector<CollisionEvent *>::iterator itrVec;

    itrVec = m_uCollisionEventList.begin();
    if (itrVec != m_uCollisionEventList.end()) {
        pObject = (*itrVec);
        m_uCollisionEventList.erase(itrVec);
    }

    return pObject;
}

unsigned int PhysicsManager::getCollisionEventCount()
{
    return m_uCollisionEventList.size();
}

void PhysicsManager::destroyCollisionEvent(CollisionEvent *_pEvent)
{
    if (_pEvent)
        delete _pEvent;
}
