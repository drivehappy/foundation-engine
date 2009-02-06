#include "physicsBoatEngine.h"

using namespace Foundation;
using namespace Foundation::Physics;
using namespace Foundation::Math;

// Static Data Members
//Task        *BoatEngine::pTaskUpdate;
//

BoatEngine::BoatEngine()
{
    pTaskUpdate = new Task(this, BoatEngine::doTaskUpdate_Wrapper);
    //pTaskUpdate->_this = this;
    //pTaskUpdate->_functionPointer = BoatEngine::doTaskUpdate_Wrapper;

    m_nPhysicsBody = PhysicsManager::getSingleton().createRigidBody();
}

BoatEngine::~BoatEngine()
{
    destroy();
}

void BoatEngine::initialize(float _nMass, float _nFriction, float _nPropellerThrust, float _nRudderTorque, float _nEngineRudderTorque, float _nKeelForce, float _nMaxVelocity, float _nAcceleration, float _nInertia, float _nRotFriction)
{
    m_nThrottle = 0.0f;
    m_nRudder   = 0.0f;
    m_nPropellerThrust  = _nPropellerThrust;
    m_nRudderTorque = _nRudderTorque;
    m_nKeelForce    = _nKeelForce;
    m_nMaxVelocity  = _nMaxVelocity;
    m_nAcceleration = _nAcceleration;
    m_nEngineRudderTorque = _nEngineRudderTorque;
    m_nRotFriction = _nRotFriction;
    m_nInertia = _nInertia;
    m_nMass = _nMass;
    m_nFriction = _nFriction;

    m_nPhysicsBody->setMass(_nMass);
    m_nPhysicsBody->setInertia(_nInertia);
    m_nPhysicsBody->setFriction(_nFriction);
    m_nPhysicsBody->setRotFriction(_nRotFriction);
    m_nPhysicsBody->setPosition(gmtl::Vec3f(0, 0, 0));
}

void BoatEngine::initialize()
{
    m_nPhysicsBody->setMass(m_nMass);
    m_nPhysicsBody->setInertia(m_nInertia);
    m_nPhysicsBody->setFriction(m_nFriction);
    m_nPhysicsBody->setRotFriction(m_nRotFriction);
    m_nPhysicsBody->setPosition(gmtl::Vec3f(0, 0, 0));
}

void BoatEngine::destroy()
{
    if (m_nPhysicsBody)
        PhysicsManager::getSingleton().destroyRigidBody(m_nPhysicsBody);
}

void* BoatEngine::doTaskUpdate(void *_pArgs)
{
    float nDeltaTime = *(float*)_pArgs;

    // DEBUGGING
    /*
    static int nStep = 0;
    char *pFileBuffer = new char[1024];
    sprintf(pFileBuffer, "../../data/BOATENGINE_%2.4f.txt", nDeltaTime);
    FileSystem::File *pFileBoatEngine = FileSystem::FileManager::getSingleton().open(pFileBuffer, FileSystem::APPEND, FileSystem::TEXT);
    */

    gmtl::Vec3f nPosition           = m_nPhysicsBody->getPosition();
    gmtl::Vec3f nVelocity           = m_nPhysicsBody->getVelocity();
    gmtl::Quatf nRotation           = m_nPhysicsBody->getRotation();
    float nAngularVelocity          = m_nPhysicsBody->getAngularVelocity();

    /*
    gmtl::Vec3f forward         = q * gmtl::Vec3f(0.0f, 0.0f, -1.0f);
    gmtl::Vec3f right           = q * gmtl::Vec3f(1.0f, 0.0f, 0.0f);

    float       force           = m_nThrottle * m_nEnginePower * nElapsedTime;
    float       velIntoKeel     = gmtl::dot(right, v);
    float       keelF           = -1.0f * velIntoKeel * m_nKeelForce * nElapsedTime;
    float       inlineVel       = gmtl::dot(forward, v);
    float       velOverRudder   = force * (1.0f - m_nEngineRudderTorque) + inlineVel;
    float       velOverRudder   = force + (inlineVel * nElapsedTime);
    float       torque          = velOverRudder * m_nRudder * m_nRudderTorque * nElapsedTime;
    */

    // Runge-Kutta method for closely approximating our linear velocity over discrete time intervals
    gmtl::Vec3f nForce;
    gmtl::Vec3f nAcceleration;
    gmtl::Vec3f nK1, nK2, nK3, nK4;
    gmtl::Vec3f nVelocityNew;
    gmtl::Vec3f nVelocityAverage;     // Riemann sum of the middle for the position, try the Runge-Kutta for better approx
    gmtl::Vec3f nPositionNew;
    gmtl::Vec3f nForward;
    gmtl::Vec3f nApproximatedForce;

    // Create a forward unit vector based on our rotation
    // Used for the linear accelerations
    nForward = m_nPhysicsBody->m_nRotation * gmtl::Vec3f(0, 0, -1);

    // Test
    nVelocity = nForward * gmtl::length(nVelocity);
    //

    nForce = (m_nThrottle * m_nPropellerThrust * nForward) - (m_nFriction * nVelocity);
    nAcceleration = nForce / m_nMass;
    nK1 = nDeltaTime * nAcceleration;

    nForce = (m_nThrottle * m_nPropellerThrust * nForward) - (m_nFriction * (nVelocity + nK1 * 0.5f));
    nAcceleration = nForce / m_nMass;
    nK2 = nDeltaTime * nAcceleration;

    nForce = (m_nThrottle * m_nPropellerThrust * nForward) - (m_nFriction * (nVelocity + nK2 * 0.5f));
    nAcceleration = nForce / m_nMass;
    nK3 = nDeltaTime * nAcceleration;

    nForce = (m_nThrottle * m_nPropellerThrust * nForward) - (m_nFriction * (nVelocity + nK3));
    nAcceleration = nForce / m_nMass;
    nK4 = nDeltaTime * nAcceleration;
    
    nApproximatedForce = (nK1 + 2.0f * nK2 + 2.0f * nK3 + nK4) / 6.0f;
    nVelocityNew = nVelocity + nApproximatedForce;
    
    nVelocityAverage = (nVelocityNew + nVelocity) * 0.5f;
    nPositionNew = nPosition + (nVelocityAverage * nDeltaTime);
    
    m_nPhysicsBody->setVelocity(nVelocityNew);
    m_nPhysicsBody->setPosition(nPositionNew);

    //f_printf("NewPosition: %f %f %f\n", nPositionNew[0], nPositionNew[1], nPositionNew[2]);
    // --


    // Angular Rotation
    gmtl::Vec3f nAngularForce;
    float nAngularDragCoefficient = m_nRotFriction; //(1.0f - m_nRotFriction);
    float nAngularAcceleration;
    float nA1, nA2, nA3, nA4;
    float nAngularVelocityNew;
    float nAngularVelocityAverage;
    gmtl::Quatf nRotationNew = gmtl::Quatf(0,0,0,1);
    gmtl::Vec3f nLeft, nRadialPosition, nTorque;
    
    float nInlineVelocity;
    float nTorque2;

    //
    //f_printf("DEBUG\n");
    gmtl::normalize(nRotation);

    //nInlineVelocity = gmtl::dot(nForward, nVelocity);




    /*
    /// A1
    // Create a force vector based on the Left vector, the rudder, the boat velocity and the angular drag coefficient
    nAngularForce = (m_nRudder * (gmtl::length(nVelocity) * gmtl::Vec3f(1, 0, 0)) * 1.0f) - (nAngularDragCoefficient * gmtl::Vec3f(nAngularVelocity, 0, 0));
    //f_printf("nAngularForce: %f, %f, %f\n", nAngularForce[gmtl::Xelt], nAngularForce[gmtl::Yelt], nAngularForce[gmtl::Zelt]);

    // Create a vector pointing away from our body to the left rotated correctly
    nLeft = nRotation * nAngularForce;
    //f_printf("nLeft: %f, %f, %f\n", nLeft[gmtl::Xelt], nLeft[gmtl::Yelt], nLeft[gmtl::Zelt]);

    // Create a position vector of test lenth 6 from the center to the origin of the Left vector at the back of the boat
    nRadialPosition = nRotation * gmtl::Vec3f(0, 0, -6);
    //f_printf("nRadialPosition: %f, %f, %f\n", nRadialPosition[gmtl::Xelt], nRadialPosition[gmtl::Yelt], nRadialPosition[gmtl::Zelt]);

    // Create the torque vector from our position vector and force vector
    gmtl::cross(nTorque, nRadialPosition, nLeft);
    //f_printf("Torque: %f, %f, %f\n", nTorque[gmtl::Xelt], nTorque[gmtl::Yelt], nTorque[gmtl::Zelt]);

    // Create our angular acceleration scalar
    nAngularAcceleration = nTorque[gmtl::Yelt] / m_nPhysicsBody->m_nInertia;
    //f_printf("nAngularAcceleration: %f\n", nAngularAcceleration);

    //nA1 = nAngularAcceleration * nDeltaTime;
    nA1 = m_nRudder * m_nRudderTorque * nDeltaTime;

    /// A2
    nAngularForce = (m_nRudder * (gmtl::length(nVelocity) * gmtl::Vec3f(1, 0, 0)) * 0.01f) - (nAngularDragCoefficient * (gmtl::Vec3f(nAngularVelocity, 0, 0) + gmtl::Vec3f(nA1, 0, 0) * 0.5f));
    nLeft = nRotation * nAngularForce;
    nRadialPosition = nRotation * gmtl::Vec3f(0, 0, -6);
    gmtl::cross(nTorque, nRadialPosition, nLeft);
    nAngularAcceleration = nTorque[gmtl::Yelt] / m_nPhysicsBody->m_nInertia;
    nA2 = nAngularAcceleration * nDeltaTime;
    nA2 = m_nRudder * m_nRudderTorque * nDeltaTime;

    /// A3
    nAngularForce = (m_nRudder * (gmtl::length(nVelocity) * gmtl::Vec3f(1, 0, 0)) * 0.01f) - (nAngularDragCoefficient * (gmtl::Vec3f(nAngularVelocity, 0, 0) + gmtl::Vec3f(nA2, 0, 0) * 0.5f));
    nLeft = nRotation * nAngularForce;
    nRadialPosition = nRotation * gmtl::Vec3f(0, 0, -6);
    gmtl::cross(nTorque, nRadialPosition, nLeft);
    nAngularAcceleration = nTorque[gmtl::Yelt] / m_nPhysicsBody->m_nInertia;
    //nA3 = nAngularAcceleration * nDeltaTime;
    nA3 = m_nRudder * m_nRudderTorque * nDeltaTime;

    /// A4
    nAngularForce = (m_nRudder * (gmtl::length(nVelocity) * gmtl::Vec3f(1, 0, 0)) * 0.01f) - (nAngularDragCoefficient * (gmtl::Vec3f(nAngularVelocity, 0, 0) + gmtl::Vec3f(nA3, 0, 0)));
    nLeft = nRotation * nAngularForce;
    nRadialPosition = nRotation * gmtl::Vec3f(0, 0, -6);
    gmtl::cross(nTorque, nRadialPosition, nLeft);
    nAngularAcceleration = nTorque[gmtl::Yelt] / m_nPhysicsBody->m_nInertia;
    //nA4 = nAngularAcceleration * nDeltaTime;
    nA4 = m_nRudder * m_nRudderTorque * nDeltaTime;
    */


    // Create our angular velocity scalar
    //nAngularVelocityNew = nAngularAcceleration * nDeltaTime;
    //nAngularVelocityNew = nAngularVelocity + (nA1 + 2.0f * nA2 + 2.0f * nA3 + nA4) / 6.0f;
    if (gmtl::length(nVelocityNew) > 1.0f)
        nAngularAcceleration = (m_nRudder * m_nRudderTorque) - (nAngularDragCoefficient * nAngularVelocity);
    else
        nAngularAcceleration = 0.0f;
    nAngularVelocityNew = nAngularAcceleration * nDeltaTime;
    //nAngularVelocityAverage = (nAngularVelocityNew + nAngularVelocity) * 0.5f;
    nAngularVelocityAverage = nAngularVelocityNew;
    //f_printf("nAngularVelocityNew: %f\n", nAngularVelocityNew);

    // Create our new rotation
    gmtl::Quatf nAngularVelocityQuat = gmtl::Quatf(0, nAngularVelocityAverage, 0, 0);
    //gmtl::normalize(nAngularVelocityQuat);
    nRotationNew = nRotation + ((nRotation * nAngularVelocityQuat));
    gmtl::normalize(nRotationNew);

    //f_printf("nRotation: %f, %f, %f, %f\n", nRotation[gmtl::Xelt], nRotation[gmtl::Yelt], nRotation[gmtl::Zelt], nRotation[gmtl::Welt]);
    //f_printf("nAngularVelocityNew: %f\n", nAngularVelocityNew);
    //f_printf("/DEBUG\n");

    m_nPhysicsBody->m_nAngularVelocity = nAngularVelocityNew;
    m_nPhysicsBody->m_nRotation = nRotationNew;

    
    // --

    // DEBUGGING
    /*
    memset(pFileBuffer, 0, 1024);
    sprintf(pFileBuffer,   "Boat: %p", this);
    pFileBoatEngine->write(pFileBuffer, strlen(pFileBuffer));

    /// Write Above Angular Temp Vars
    memset(pFileBuffer, 0, 1024);
    sprintf(pFileBuffer,   "\nAngular Vars:\n Angular Force:\t\t\t%f, %f, %f\n Left Force:\t\t\t%f, %f, %f\n Radial Position:\t\t%f, %f, %f\n nTorque:\t\t\t\t%f, %f ,%f\n nAngularVelocityNew:\t%f\n nRotationNew:\t\t\t%f, %f, %f, %f\n",
                            nAngularForce[gmtl::Xelt], nAngularForce[gmtl::Yelt], nAngularForce[gmtl::Zelt], 
                            nLeft[gmtl::Xelt], nLeft[gmtl::Yelt], nLeft[gmtl::Zelt], 
                            nRadialPosition[gmtl::Xelt], nRadialPosition[gmtl::Yelt], nRadialPosition[gmtl::Zelt],
                            nTorque[gmtl::Xelt], nTorque[gmtl::Yelt], nTorque[gmtl::Zelt], 
                            nAngularVelocityNew,
                            nRotationNew[gmtl::Xelt], nRotationNew[gmtl::Yelt], nRotationNew[gmtl::Zelt], nRotationNew[gmtl::Welt]);
    pFileBoatEngine->write(pFileBuffer, strlen(pFileBuffer));

    /// Write Above Linear Temp Vars
    memset(pFileBuffer, 0, 1024);
    sprintf(pFileBuffer,   "\nLinear Vars:\n position:\t\t\t\t%f, %f, %f\n velocity:\t\t\t\t%f, %f, %f\n rotation:\t\t\t\t%f, %f, %f, %f\n\n nForce:\t\t\t\t%f, %f, %f\n",
                            nPosition[gmtl::Xelt], nPosition[gmtl::Yelt], nPosition[gmtl::Zelt], 
                            nVelocity[gmtl::Xelt], nVelocity[gmtl::Yelt], nVelocity[gmtl::Zelt], 
                            nRotation[gmtl::Xelt], nRotation[gmtl::Yelt], nRotation[gmtl::Zelt], nRotation[gmtl::Welt], 
                            nForce[gmtl::Xelt], nForce[gmtl::Yelt], nForce[gmtl::Zelt]);
    pFileBoatEngine->write(pFileBuffer, strlen(pFileBuffer));

    /// Write Above Linear Temp Vars
    memset(pFileBuffer, 0, 1024);
    sprintf(pFileBuffer,   "\nNew Linear Vars:\n newPosition:\t\t\t\t%f, %f, %f\n newVelocity:\t\t\t\t%f, %f, %f\n-----------------------------------------------\n",
                            nPositionNew[gmtl::Xelt], nPositionNew[gmtl::Yelt], nPositionNew[gmtl::Zelt], 
                            nVelocityNew[gmtl::Xelt], nVelocityNew[gmtl::Yelt], nVelocityNew[gmtl::Zelt]); 
    pFileBoatEngine->write(pFileBuffer, strlen(pFileBuffer));

    nStep++;
    
    pFileBoatEngine->close();
    delete [] pFileBuffer;
    */
    // --

    //f_printf("Keel Force: %f\n Force: %f\nTorque: %f\n", keelF, -force, torque);

    //m_nPhysicsBody->applyAlignedForce(gmtl::Vec3f(keelF, 0.0f, -force));
    //m_nPhysicsBody->applyAlignedForce(gmtl::Vec3f(0.0f, 0.0f, -nForce));
    //m_nPhysicsBody->applyAlignedTorque(gmtl::Vec3f(0.0f, torque, 0.0f));
    //m_nPhysicsBody->update(nDeltaTime);

    return NULL;
}

gmtl::Vec3f BoatEngine::getPosition() const
{
    if (m_nPhysicsBody)
        return m_nPhysicsBody->getPosition();

    f_printf("[BoatEngine] Warning: No physics body attached, could not get position.\n");
    return gmtl::Vec3f(0, 0, 0);
};

gmtl::Quatf BoatEngine::getRotation() const
{
    if (m_nPhysicsBody)
        return m_nPhysicsBody->getRotation();

    f_printf("[BoatEngine] Warning: No physics body attached, coult not get rotation.\n");
    return gmtl::Quatf(0, 0, 0, 1);
};

void BoatEngine::setPosition(gmtl::Vec3f _nPosition)
{
    if (m_nPhysicsBody)
        m_nPhysicsBody->setPosition(_nPosition);
}

void BoatEngine::setRotation(gmtl::Quatf _nRotation)
{
    if (m_nPhysicsBody)
        m_nPhysicsBody->setRotation(_nRotation);
}

float BoatEngine::getRudder()
{
    return m_nRudder;
}

void BoatEngine::setRudder(float _nRudder)
{
    float nChange = _nRudder - m_nRudder;

    nChange = f_clamp(nChange, -2.0f, 2.0f);
    m_nRudder += nChange;
    m_nRudder = f_clamp(m_nRudder, -1.0f, 1.0f);
}

float BoatEngine::getThrottle()
{
    return m_nThrottle;
}

gmtl::Vec3f BoatEngine::getVelocity() const
{
    return m_nPhysicsBody->getVelocity();
}

void BoatEngine::setVelocity(const gmtl::Vec3f _nVelocity)
{
    m_nPhysicsBody->setVelocity(_nVelocity);
}

void BoatEngine::reset()
{
    m_nThrottle = 0.0f;
    m_nRudder   = 0.0f;

    m_nPhysicsBody->reset();
    m_nPhysicsBody->setMass(m_nMass);
    m_nPhysicsBody->setInertia(m_nInertia);
    m_nPhysicsBody->setFriction(m_nFriction);
    m_nPhysicsBody->setRotFriction(m_nRotFriction);

    //f_printf("BoatEngine reset\n");
}
