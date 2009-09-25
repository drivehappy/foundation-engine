#include <boost/python.hpp>
using namespace boost::python;

#include "foundation.h"
using namespace Foundation;

#include "py_communicator.h"
#include "py_gmtl.h"
#include "py_physicsSystem.h"
#include "py_graphicManager.h"
#include "py_guiSystem.h"
#include "py_fileSystem.hpp"
#include "py_debug.hpp"
#include "py_audioSystem.h"
#include "py_entitySystem.h"
#include "py_sphereTree.h"
#include "py_terrainSystem.h"
#include "py_scheduler.h"

BOOST_PYTHON_MODULE(FoundationPython)
{
    // Terrain
    class_<Terrain::TerrainManager_PythonWrapper, boost::noncopyable>("TerrainManager")
        .def("create", &Terrain::TerrainManager_PythonWrapper::create)
        .def("getRayIntersection", &Terrain::TerrainManager_PythonWrapper::getRayIntersection)
    ;

    // Math Helpers
    def("f_max", Math::f_max<float>);
    def("f_min", Math::f_min<float>);
    def("f_clamp", Math::f_clamp<float>);

    // Debug_PythonWrapper
    class_<CPUInfo_PythonWrapper, boost::noncopyable>("CPUInfo")
        .def("getVendorName", &CPUInfo_PythonWrapper::getVendorName)
        .def("getStepping", &CPUInfo_PythonWrapper::getStepping)
        .def("getModel", &CPUInfo_PythonWrapper::getModel)
        .def("getFamily", &CPUInfo_PythonWrapper::getFamily)
        .def("getType", &CPUInfo_PythonWrapper::getType)
        .def("getExtendedModel", &CPUInfo_PythonWrapper::getExtendedModel)
        .def("getExtendedFamily", &CPUInfo_PythonWrapper::getExtendedFamily)
    ;
    class_<Debug_PythonWrapper, boost::noncopyable>("Debug")
        .def("getCPUInfo", &Debug_PythonWrapper::getCPUInfo)
    ;

    // TimeManager
    class_<TimeManager_PythonWrap, boost::noncopyable>("TimeManager")
        .def("getTime", &TimeManager_PythonWrap::getTime)
        .def("sleep", &TimeManager_PythonWrap::sleep)
        .def("setSimulationFactor", &TimeManager_PythonWrap::setSimulationFactor)
    ;

    // Timer
    class_<Timer, boost::noncopyable>("Timer")
        .def("pause", &Timer::pause)
        .def("resume", &Timer::resume)
        .def("reset", &Timer::reset)
        .def("getTime", &Timer::getTime)
    ;

    // FileSystem
    enum_<FileSystem::FileAccess>("FileAccess")
        .value("READ", FileSystem::READ)
        .value("WRITE", FileSystem::WRITE)
        .value("APPEND", FileSystem::APPEND)
        .value("READ_WRITE_EXISTS", FileSystem::READ_WRITE_EXISTS)
        .value("READ_WRITE_CREATE", FileSystem::READ_WRITE_CREATE)
        .value("READ_APPEND", FileSystem::READ_APPEND)
    ;
    enum_<FileSystem::FileMode>("FileMode")
        .value("BINARY", FileSystem::BINARY)
        .value("TEXT", FileSystem::TEXT)
    ;
    class_<FileSystem::File_PythonWrapper, boost::noncopyable>("File")
        .def("read", &FileSystem::File_PythonWrapper::read)
        .def("write", &FileSystem::File_PythonWrapper::write)
    ;
    class_<FileSystem::FileManager_PythonWrapper, boost::noncopyable>("FileManager")
        .def("open", &FileSystem::FileManager_PythonWrapper::open, return_internal_reference<>())
        .def("close", &FileSystem::FileManager_PythonWrapper::close)
    ;

    // Communicator
    void (Channel_PythonWrapper::*join1)(const char *_pChannelName, PyObject *_pFunction) = &Channel_PythonWrapper::Channel_Join;
    void (Channel_PythonWrapper::*join2)(const char *_pChannelName, PyObject *_pClass, const char *_sMethodName) = &Channel_PythonWrapper::Channel_Join;
    void (Channel_PythonWrapper::*leave1)(const char *_pChannelName, PyObject *_pFunction) = &Channel_PythonWrapper::Channel_Leave;
    void (Channel_PythonWrapper::*leave2)(const char *_pChannelName, PyObject *_pClass, const char *_sMethodName) = &Channel_PythonWrapper::Channel_Leave;

    class_<Channel_PythonWrapper, boost::noncopyable>("Channel")
        .def("Channel_MessageCreate", &Channel_PythonWrapper::Channel_MessageCreate)
        .def("Channel_MessageDestroy", &Channel_PythonWrapper::Channel_MessageDestroy)
        .def("Channel_MessageHeader", &Channel_PythonWrapper::Channel_MessageHeader)
        .def("Channel_MessageData", &Channel_PythonWrapper::Channel_MessageData)

        .def("Channel_Join", join1)
        .def("Channel_Join", join2)
        .def("Channel_Leave", leave1)
        .def("Channel_Leave", leave2)
        .def("Channel_Send", &Channel_PythonWrapper::Channel_Send)
    ;

    // Scheduler
    class_<Task, boost::noncopyable>("CPPTask")
        .def_readwrite("_dTime", &Task::_dTime)
        .def_readwrite("_dLastTime", &Task::_dLastTime)
    ;
    class_< Task_PythonWrapper, bases<Task> >("Task", init<PyObject*>())
        .def(init<PyObject*, const char *>())

        .def_readwrite("_kill", &Task_PythonWrapper::_kill)
        .def_readwrite("_this", &Task_PythonWrapper::_this)
        //.def_readwrite("_functionPointer", &Task::_functionPointer)
        .def_readwrite("_dTime", &Task_PythonWrapper::_dTime)
        .def_readwrite("_dLastTime", &Task_PythonWrapper::_dLastTime)
    ;

    void (Scheduler_PythonWrapper::*AddTask1)(Task_PythonWrapper *_task, unsigned int _tickStep, unsigned int _tickCount) = &Scheduler_PythonWrapper::AddTask;
    void (Scheduler_PythonWrapper::*AddTask2)(Task *_task, unsigned int _tickStep, unsigned int _tickCount) = &Scheduler_PythonWrapper::AddTask;
    void (Scheduler_PythonWrapper::*RemoveTask1)(Task_PythonWrapper *_task) = &Scheduler_PythonWrapper::RemoveTask;
    void (Scheduler_PythonWrapper::*RemoveTask2)(Task *_task) = &Scheduler_PythonWrapper::RemoveTask;
    class_<Scheduler_PythonWrapper, boost::noncopyable>("Scheduler")
        .def("AddTask", AddTask1)
        .def("AddTask", AddTask2)
        .def("RemoveTask", RemoveTask1)
        .def("RemoveTask", RemoveTask2)
        //.def("AddTask", &Scheduler_PythonWrapper::AddTask)
        //.def("RemoveTask", &Scheduler_PythonWrapper::RemoveTask)

        .def("KillTask", &Scheduler_PythonWrapper::KillTask)
        .def("RunTask", &Scheduler_PythonWrapper::RunTask)
        .def("Step", &Scheduler_PythonWrapper::Step)
        .def("SetStepTiming", &Scheduler_PythonWrapper::SetStepTiming)
        .def("SetTaskEnable", &Scheduler_PythonWrapper::SetTaskEnable)
        .def("StepTask", &Scheduler_PythonWrapper::StepTask)
    ;

	// Entity, Graphics and Manager
    class_<Entities::EntityGraphic_PythonWrapper>("EntityGraphic", init<const char *, const char *>())
        .def("setMesh", &Entities::EntityGraphic_PythonWrapper::setMesh)
        .def("setScale", &Entities::EntityGraphic_PythonWrapper::setScale)
        .def("setRotation", &Entities::EntityGraphic_PythonWrapper::setRotation)
        .def("setPosition", &Entities::EntityGraphic_PythonWrapper::setPosition)
        .def("setMaterial", &Entities::EntityGraphic_PythonWrapper::setMaterial)
        .def("getPosition", &Entities::EntityGraphic_PythonWrapper::getPosition)
    ;
    class_<Entities::Entity_PythonWrapper, boost::noncopyable>("Entity", no_init)
        .def("getId", &Entities::Entity_PythonWrapper::getId)
        .def("setGraphics", &Entities::Entity_PythonWrapper::setGraphics)
        .def("getGraphics", &Entities::Entity_PythonWrapper::getGraphics, return_internal_reference<>())
        .def("getName", &Entities::Entity_PythonWrapper::getName)
        .def("setName", &Entities::Entity_PythonWrapper::setName)
    ;
	class_<Entities::EntityManager_PythonWrapper, boost::noncopyable>("EntityManager")
        .def("getTask", &Entities::EntityManager_PythonWrapper::getTask, return_internal_reference<>())
		.def("addEntity", &Entities::EntityManager_PythonWrapper::addEntity, return_internal_reference<>())
	;

    // GMTL
    enum_<gmtl::VectorIndex>("VectorIndex")
        .value("Xelt", gmtl::Xelt)
        .value("Yelt", gmtl::Yelt)
        .value("Zelt", gmtl::Zelt)
    ;
    class_<gmtl::Vec2f>("Vec2f", init<float, float>())
        .def(init<gmtl::Vec2f &>())
    ;
    class_<gmtl::Vec3f>("Vec3f", init<float, float, float>())
        .def(init<gmtl::Vec3f &>())
    ;
    class_<gmtl::Vec4f>("Vec4f", init<float, float, float, float>())
        .def(init<gmtl::Vec4f &>())
    ;
    class_<gmtl::Quatf>("Quatf", init<float, float, float, float>())
        .def(init<gmtl::Quatf &>())
    ;
    class_<gmtlVector2_PythonWrapper, bases<gmtl::Vec2f> >("Vector2", init<float, float>())
        .def(init<gmtl::Vec2f &>())
        .def(init<>())

        .def("__getitem__", &gmtlVector2_PythonWrapper::get)
        .def("__setitem__", &gmtlVector2_PythonWrapper::set)
        .def("__sub__", &gmtlVector2_PythonWrapper::operator-)
        .def("__add__", &gmtlVector2_PythonWrapper::operator+)
        .def("__div__", &gmtlVector2_PythonWrapper::operator/)
        .def("__mul__", &gmtlVector2_PythonWrapper::operator*)

        .def("length", &gmtlVector2_PythonWrapper::length)
        .def("normalize", &gmtlVector2_PythonWrapper::normalize)
        

        .def_pickle(vector_pickle_suite())
    ;
    class_<gmtlVector3_PythonWrapper, bases<gmtl::Vec3f> >("Vector3", init<float, float, float>())
        .def(init<gmtl::Vec3f &>())
        .def(init<>())

        .def("__getitem__", &gmtlVector3_PythonWrapper::get)
        .def("__setitem__", &gmtlVector3_PythonWrapper::set)
        .def("__sub__", &gmtlVector3_PythonWrapper::operator-)
        .def("__add__", &gmtlVector3_PythonWrapper::operator+)
        .def("__div__", &gmtlVector3_PythonWrapper::operator/)
        .def("__mul__", &gmtlVector3_PythonWrapper::operator*)

        .def("length", &gmtlVector3_PythonWrapper::length)
        .def("normalize", &gmtlVector3_PythonWrapper::normalize)

        .def_pickle(vector_pickle_suite())
    ;
    class_<gmtlVector4_PythonWrapper, bases<gmtl::Vec4f> >("Vector4", init<float, float, float, float>())
        .def(init<gmtl::Vec4f &>())
        .def(init<>())

        .def("__getitem__", &gmtlVector4_PythonWrapper::get)
        .def("__setitem__", &gmtlVector4_PythonWrapper::set)
        .def("__sub__", &gmtlVector4_PythonWrapper::operator-)
        .def("__add__", &gmtlVector4_PythonWrapper::operator+)
        .def("__div__", &gmtlVector4_PythonWrapper::operator/)
        .def("__mul__", &gmtlVector4_PythonWrapper::operator*)

        .def("length", &gmtlVector4_PythonWrapper::length)
        .def("normalize", &gmtlVector4_PythonWrapper::normalize)

        .def_pickle(vector_pickle_suite())
    ;
    class_<gmtlQuaternion_PythonWrapper, bases<gmtl::Quatf> >("Quaternion", init<float, float, float, float>())
        .def(init<gmtl::Quatf &>())
        .def(init<>())

        .def("getAngleY", &gmtlQuaternion_PythonWrapper::getAngleY)
        .def("rotate", &gmtlQuaternion_PythonWrapper::rotate)
		.def("normalize", &gmtlQuaternion_PythonWrapper::normalize)

        .def("__getitem__", &gmtlQuaternion_PythonWrapper::get)
        .def("__mul__", &gmtlQuaternion_PythonWrapper::operator*)
        .def("__add__", &gmtlQuaternion_PythonWrapper::operator+)
        .def("__sub__", &gmtlQuaternion_PythonWrapper::operator-)

        .def_pickle(quaternion_pickle_suite())
    ;

    // Network
    // Server
    class_<Network::ClientInfo, boost::noncopyable>("ClientInfo")
        .def_readonly("totalRecvData", &Network::ClientInfo::m_nTotalRecvData)
        .def_readonly("totalSentData", &Network::ClientInfo::m_nTotalSentData)

        .def("setAddress", &Network::ClientInfo::setAddress)
        .def("getAddress", &Network::ClientInfo::getAddress)

        .def("__eq__", &Network::ClientInfo::operator==)
    ;

    /// Create member function variables for overloaded functions
    void (Network::Server::*send1)(SystemAddress _addr, const char *_pData, unsigned int _nLength) = &Network::Server::send;
    void (Network::Server::*send2)(Network::ClientInfo _client, const char *_pData, unsigned int _nLength) = &Network::Server::send;

    class_<Network::Server, boost::noncopyable>("Server")
        .def("listen", &Network::Server::listen)
        .def("send", send1)
        .def("send", send2)
        .def("getTaskListen", &Network::Server::getTaskListen, return_internal_reference<>())
        .def("close", &Network::Server::close)
        .def("shutdown", &Network::Server::shutdown)
        .def("getBitsPerSecond", &Network::Server::getBitsPerSecond)
        .def("getClientCount", &Network::Server::getClientCount)
    ;

    // Client
    class_<Network::Client, boost::noncopyable>("Client")
        .def("connect", &Network::Client::connect)
        .def("close", &Network::Client::close)
        .def("send", &Network::Client::send)
        .def("getTaskReceive", &Network::Client::getTaskReceive, return_internal_reference<>())
        .def("getLocalPort", &Network::Client::getLocalPort)
        .def("getBitsPerSecond", &Network::Client::getBitsPerSecond)
    ;

    // Physics
    class_<Physics::PhysicsDebugObject, boost::noncopyable>("PhysicsDebugObject", no_init)
    ;
    class_<Physics::RigidBody_PythonWrapper, boost::noncopyable>("RigidBody")
        .def("applyForce", &Physics::RigidBody_PythonWrapper::applyForce)

        .def("getPosition", &Physics::RigidBody_PythonWrapper::getPosition)
        .def("getRotation", &Physics::RigidBody_PythonWrapper::getRotation)
    ;

    void (Physics::BoatEngine_PythonWrapper::*initialize1)(float _nMass, float _nFriction, float _nEnginePower, float _nRudderTorque, float _nEngineRudderTorque, float _nKeelForce, float _nMaxVelocity, float _nAcceleration, float _nInertia, float _nRotFriction) = &Physics::BoatEngine_PythonWrapper::initialize;
    void (Physics::BoatEngine_PythonWrapper::*initialize2)() = &Physics::BoatEngine_PythonWrapper::initialize;
    class_<Physics::BoatEngine_PythonWrapper, boost::noncopyable>("BoatEngine")
        .def("initialize", initialize1)
        .def("initialize", initialize2)

        .def("destroy", &Physics::BoatEngine_PythonWrapper::destroy)
        .def("getTaskUpdate", &Physics::BoatEngine_PythonWrapper::getTaskUpdate, return_internal_reference<>())

        .def("setThrottle", &Physics::BoatEngine_PythonWrapper::setThrottle)
        .def("setRudder", &Physics::BoatEngine_PythonWrapper::setRudder)

        .def("getPosition", &Physics::BoatEngine_PythonWrapper::getPosition)
        .def("getRotation", &Physics::BoatEngine_PythonWrapper::getRotation)

        .def("setPosition", &Physics::BoatEngine_PythonWrapper::setPosition)
        .def("setRotation", &Physics::BoatEngine_PythonWrapper::setRotation)

        .def("getRudder", &Physics::BoatEngine_PythonWrapper::getRudder)
        .def("getThrottle", &Physics::BoatEngine_PythonWrapper::getThrottle)

        .def("getVelocity", &Physics::BoatEngine_PythonWrapper::getVelocity)
        .def("setVelocity", &Physics::BoatEngine_PythonWrapper::setVelocity)

        .def("reset", &Physics::BoatEngine_PythonWrapper::reset)

        //.def_pickle(Physics::boatengine_pickle_suite())
    ;

    void (Physics::PhysicsManager_PythonWrap::*setCollisionShapeRotation1)(unsigned int _nId, gmtlQuaternion_PythonWrapper _nRotation) = &Physics::PhysicsManager_PythonWrap::setCollisionShapeRotation;
    void (Physics::PhysicsManager_PythonWrap::*setCollisionShapeRotation2)(unsigned int _nId, gmtlVector3_PythonWrapper _nAxis, float _nAngle) = &Physics::PhysicsManager_PythonWrap::setCollisionShapeRotation;
    class_<Physics::PhysicsManager_PythonWrap, boost::noncopyable>("PhysicsManager")
        .def("setPaused", &Physics::PhysicsManager_PythonWrap::setPaused)
        .def("getTaskUpdate", &Physics::PhysicsManager_PythonWrap::getTaskUpdate, return_internal_reference<>())
        .def("initialize", &Physics::PhysicsManager_PythonWrap::initialize)
        .def("destroy", &Physics::PhysicsManager_PythonWrap::destroy)
        .def("createRigidBody", &Physics::PhysicsManager_PythonWrap::createRigidBody, return_internal_reference<>())
        .def("destroyRigidBody", &Physics::PhysicsManager_PythonWrap::destroyRigidBody)
        .def("createCollisionShape", &Physics::PhysicsManager_PythonWrap::createCollisionShape)
        .def("destroyCollisionObject", &Physics::PhysicsManager_PythonWrap::destroyCollisionObject)
        .def("consumeCollisionEvent", &Physics::PhysicsManager_PythonWrap::consumeCollisionEvent, return_internal_reference<>())
        .def("getCollisionEventCount", &Physics::PhysicsManager_PythonWrap::getCollisionEventCount)
        .def("setCollisionShapeOrigin", &Physics::PhysicsManager_PythonWrap::setCollisionShapeOrigin)
        .def("getCollisionShapeOrigin", &Physics::PhysicsManager_PythonWrap::getCollisionShapeOrigin)
        .def("setCollisionShapeRotation", setCollisionShapeRotation1)
        .def("setCollisionShapeRotation", setCollisionShapeRotation2)
        .def("destroyCollisionEvent", &Physics::PhysicsManager_PythonWrap::destroyCollisionEvent)
    ;
    class_<Physics::CollisionEvent_PythonWrapper, boost::noncopyable>("CollisionEvent", no_init)
        .def("getBody0", &Physics::CollisionEvent_PythonWrapper::getBody0)
        .def("getBody1", &Physics::CollisionEvent_PythonWrapper::getBody1)
        .def("getContactPoint", &Physics::CollisionEvent_PythonWrapper::getContactPoint)
    ;
    enum_<Physics::CollisionShapeId>("CollisionShapeId")
        .value("BOX", Physics::BOX)
        .value("SPHERE", Physics::SPHERE)
    ;

    // Graphics
    #if defined(S_ENABLE_GRAPHIC)

    class_<Graphic::Camera_PythonWrapper, boost::noncopyable>("Camera", init<const char *, Graphic::SceneManager *>())
        .def("getPosition", &Graphic::Camera_PythonWrapper::getPosition)
        .def("setPosition", &Graphic::Camera_PythonWrapper::setPosition)
        .def("setRotation", &Graphic::Camera_PythonWrapper::setRotation)
        .def("setLookAt", &Graphic::Camera_PythonWrapper::setLookAt)
        .def("yaw", &Graphic::Camera_PythonWrapper::yaw)
        .def("pitch", &Graphic::Camera_PythonWrapper::pitch)
        .def("getPosition", &Graphic::Camera_PythonWrapper::getPosition)
        .def("getRotation", &Graphic::Camera_PythonWrapper::getRotation)
        .def("moveRelative", &Graphic::Camera_PythonWrapper::moveRelative)
        .def("isObjectVisible", &Graphic::Camera_PythonWrapper::isObjectVisible)
        .def("setAspectRatio", &Graphic::Camera_PythonWrapper::setAspectRatio)
    ;
    class_<Graphic::RenderWindow_PythonWrapper, boost::noncopyable>("RenderWindow", no_init)
        .def("getWidth", &Graphic::RenderWindow_PythonWrapper::getWidth)
        .def("getHeight", &Graphic::RenderWindow_PythonWrapper::getHeight)
    ;

    void (Graphic::GraphicManager_PythonWrapper::*setMeshRotation1)(const char *_sSceneManagerName, const char *_sID, gmtl::Quatf _nRotation) = &Graphic::GraphicManager_PythonWrapper::setMeshRotation;
    void (Graphic::GraphicManager_PythonWrapper::*setMeshRotation2)(const char *_sSceneManagerName, const char *_sID, const gmtl::Vec3f _nAxis, const float _nRadianAngle) = &Graphic::GraphicManager_PythonWrapper::setMeshRotation;
    class_<Graphic::GraphicManager_PythonWrapper, boost::noncopyable>("GraphicManager")
        .def("initialize", &Graphic::GraphicManager_PythonWrapper::initialize)
        .def("destroy", &Graphic::GraphicManager_PythonWrapper::destroy)

        .def("configure", &Graphic::GraphicManager_PythonWrapper::configure)
        .def("setupResources", &Graphic::GraphicManager_PythonWrapper::setupResources)
        .def("loadResources", &Graphic::GraphicManager_PythonWrapper::loadResources)
        .def("createScene", &Graphic::GraphicManager_PythonWrapper::createScene)

        .def("showCursor", &Graphic::GraphicManager_PythonWrapper::showCursor)

        .def("createLight", &Graphic::GraphicManager_PythonWrapper::createLight)

        .def("clearBloomCompositors", &Graphic::GraphicManager_PythonWrapper::clearBloomCompositors)
        .def("enableBloom", &Graphic::GraphicManager_PythonWrapper::enableBloom)

        .def("setShadowTechnique", &Graphic::GraphicManager_PythonWrapper::setShadowTechnique)

        .def("getTaskRender", &Graphic::GraphicManager_PythonWrapper::getTaskRender, return_internal_reference<>())

		.def("addParticleSystem", &Graphic::GraphicManager_PythonWrapper::addParticleSystem)
        .def("destroyParticleSystem", &Graphic::GraphicManager_PythonWrapper::destroyParticleSystem)
        .def("enableParticleSystemEmitter", &Graphic::GraphicManager_PythonWrapper::enableParticleSystemEmitter)
		.def("showParticleSystem", &Graphic::GraphicManager_PythonWrapper::showParticleSystem)
		.def("translateParticleSystem", &Graphic::GraphicManager_PythonWrapper::translateParticleSystem)
		.def("rotateParticleSystem", &Graphic::GraphicManager_PythonWrapper::rotateParticleSystem)
		.def("fastForwardParticleSystem", &Graphic::GraphicManager_PythonWrapper::fastForwardParticleSystem)

        .def("setCameraPosition", &Graphic::GraphicManager_PythonWrapper::setCameraPosition)
        .def("setCameraRotation", &Graphic::GraphicManager_PythonWrapper::setCameraRotation)
        .def("setCameraDirection", &Graphic::GraphicManager_PythonWrapper::setCameraDirection)
        .def("setCameraLookat", &Graphic::GraphicManager_PythonWrapper::setCameraLookat)
        .def("setCameraTracking", &Graphic::GraphicManager_PythonWrapper::setCameraTracking)

        .def("getCameraPosition", &Graphic::GraphicManager_PythonWrapper::getCameraPosition)
        .def("getCameraRotation", &Graphic::GraphicManager_PythonWrapper::getCameraRotation)
        .def("getCamera", &Graphic::GraphicManager_PythonWrapper::getCamera, return_internal_reference<>())
        .def("getCamera", &Graphic::GraphicManager_PythonWrapper::getCamera, return_internal_reference<>())

        
        .def("getRenderWindow", &Graphic::GraphicManager_PythonWrapper::getRenderWindow, return_internal_reference<>())
        .def("getWindowHandle", &Graphic::GraphicManager_PythonWrapper::getWindowHandle)

        .def("addMesh", &Graphic::GraphicManager_PythonWrapper::addMesh)
        .def("removeMesh", &Graphic::GraphicManager_PythonWrapper::removeMesh)
        .def("setMeshPosition", &Graphic::GraphicManager_PythonWrapper::setMeshPosition)
        .def("setMeshRotation", setMeshRotation1)
        .def("setMeshRotation", setMeshRotation2)
        .def("getMeshRotation", &Graphic::GraphicManager_PythonWrapper::getMeshRotation)
        .def("setMeshScale", &Graphic::GraphicManager_PythonWrapper::setMeshScale)
        .def("setMeshMaterial", &Graphic::GraphicManager_PythonWrapper::setMeshMaterial)
        .def("setMeshVisibility", &Graphic::GraphicManager_PythonWrapper::setMeshVisibility)

        .def("destroyAllEntities", &Graphic::GraphicManager_PythonWrapper::destroyAllEntities)

        .def("setSkybox", &Graphic::GraphicManager_PythonWrapper::setSkybox)

        .def("addLine", &Graphic::GraphicManager_PythonWrapper::addLine)
        .def("clearLine", &Graphic::GraphicManager_PythonWrapper::clearLine)
        .def("destroyLine", &Graphic::GraphicManager_PythonWrapper::destroyLine)
        .def("updateLine", &Graphic::GraphicManager_PythonWrapper::updateLine)

		.def("addCircle", &Graphic::GraphicManager_PythonWrapper::addCircle)
        .def("destroyCircle", &Graphic::GraphicManager_PythonWrapper::destroyCircle)
        .def("updateCircle", &Graphic::GraphicManager_PythonWrapper::updateCircle)

        .def("addPlane", &Graphic::GraphicManager_PythonWrapper::addPlane)

        .def("addSceneManager", &Graphic::GraphicManager_PythonWrapper::addSceneManager)

        .def("addCamera", &Graphic::GraphicManager_PythonWrapper::addCamera, return_internal_reference<>())
        .def("removeCamera", &Graphic::GraphicManager_PythonWrapper::removeCamera)

        .def("getAverageFPS", &Graphic::GraphicManager_PythonWrapper::getAverageFPS)

        .def("doPicking", &Graphic::GraphicManager_PythonWrapper::doPicking)
        .def("consumeSelectedObjectName", &Graphic::GraphicManager_PythonWrapper::consumeSelectedObjectName)

        .def("createRibbonTrail", &Graphic::GraphicManager_PythonWrapper::createRibbonTrail)
        .def("addObjectRibbonTrail", &Graphic::GraphicManager_PythonWrapper::addObjectRibbonTrail)
        .def("setRibbonTrailAttributes", &Graphic::GraphicManager_PythonWrapper::setRibbonTrailAttributes)
        .def("destroyRibbonTrail", &Graphic::GraphicManager_PythonWrapper::destroyRibbonTrail)
    ;

    class_<Graphic::GUIManager_PythonWrapper, boost::noncopyable>("GUIManager")
        .def("initialize", &Graphic::GUIManager_PythonWrapper::initialize)
        .def("destroy", &Graphic::GUIManager_PythonWrapper::destroy)

        .def("injectMouseEvent", &Graphic::GUIManager_PythonWrapper::injectMouseEvent)
        .def("injectKeyboardEvent", &Graphic::GUIManager_PythonWrapper::injectKeyboardEvent)

        .def("setMousePosition", &Graphic::GUIManager_PythonWrapper::setMousePosition)

        .def("createWindow", &Graphic::GUIManager_PythonWrapper::createWindow)
        .def("createButton", &Graphic::GUIManager_PythonWrapper::createButton)
        .def("createStaticText", &Graphic::GUIManager_PythonWrapper::createStaticText)
        .def("setWidgetCaption", &Graphic::GUIManager_PythonWrapper::setWidgetCaption)
        .def("setObjectColor", &Graphic::GUIManager_PythonWrapper::setObjectColor)
        .def("setTextFont", &Graphic::GUIManager_PythonWrapper::setTextFont)

        .def("destroyObject", &Graphic::GUIManager_PythonWrapper::destroyObject)

        .def("setActiveViewport", &Graphic::GUIManager_PythonWrapper::setActiveViewport)

        .def("setWidgetVisible", &Graphic::GUIManager_PythonWrapper::setWidgetVisible)
    ;

    // Water - Hydrax
    class_<Graphic::Water, boost::noncopyable>("Water")
        .def("initialize", &Graphic::Water::initialize)
        .def("destroy", &Graphic::Water::destroy)
        .def("getTaskRender", &Graphic::Water::getTaskRender, return_internal_reference<>())
    ;

    #endif  // S_ENABLE_GRAPHIC

    #if defined(S_ENABLE_AUDIO)
    // Audio
    class_<Audio::Sound_PythonWrapper, boost::noncopyable>("Sound", no_init)
    ;
    class_<Audio::Channel_PythonWrapper, boost::noncopyable>("AudioChannel", no_init)
    ;
    class_<Audio::AudioManager_PythonWrapper, boost::noncopyable>("AudioManager")
        .def("initialize", &Audio::AudioManager_PythonWrapper::initialize)
        .def("destroy", &Audio::AudioManager_PythonWrapper::destroy)
        .def("getTaskUpdate", &Audio::AudioManager_PythonWrapper::getTaskUpdate, return_internal_reference<>())
        .def("createSound", &Audio::AudioManager_PythonWrapper::createSound, return_internal_reference<>())
        .def("playSound", &Audio::AudioManager_PythonWrapper::playSound, return_internal_reference<>())
        .def("stopSound", &Audio::AudioManager_PythonWrapper::stopSound, return_internal_reference<>())
        .def("stopAllSounds", &Audio::AudioManager_PythonWrapper::stopAllSounds)
        .def("getDriverCount", &Audio::AudioManager_PythonWrapper::getDriverCount)
        .def("getDriverInfo", &Audio::AudioManager_PythonWrapper::getDriverInfo)
        .def("setDriver", &Audio::AudioManager_PythonWrapper::setDriver)
    ;
    #endif  // S_ENABLE_AUDIO

    // Input
    enum_<Input::Events>("Events")
        .value("MOUSE_MOVE", Input::MOUSE_MOVE)
        .value("MOUSE_BUTTON_PRESSED", Input::MOUSE_BUTTON_PRESSED)
        .value("MOUSE_BUTTON_RELEASED", Input::MOUSE_BUTTON_RELEASED)
        .value("KEYBOARD_KEY_PRESSED", Input::KEYBOARD_KEY_PRESSED)
        .value("KEYBOARD_KEY_RELEASED", Input::KEYBOARD_KEY_RELEASED)
        .value("JOYSTICK_MOVE", Input::JOYSTICK_MOVE)
        .value("JOYSTICK_BUTTON_PRESSED", Input::JOYSTICK_BUTTON_PRESSED)
        .value("JOYSTICK_BUTTON_RELEASED", Input::JOYSTICK_BUTTON_RELEASED)
    ;
    enum_<Input::Keycode>("Keycode")
        .value("ESCAPE", Input::KC_ESCAPE)
        .value("NUMPAD_1", Input::KC_NUMPAD1)
        .value("NUMPAD_2", Input::KC_NUMPAD2)
        .value("NUMPAD_3", Input::KC_NUMPAD3)
        .value("NUMPAD_4", Input::KC_NUMPAD4)
        .value("NUMPAD_5", Input::KC_NUMPAD5)
        .value("NUMPAD_6", Input::KC_NUMPAD6)
        .value("NUMPAD_7", Input::KC_NUMPAD7)
        .value("NUMPAD_8", Input::KC_NUMPAD8)
        .value("NUMPAD_9", Input::KC_NUMPAD9)
        .value("NUMPAD_0", Input::KC_NUMPAD0)
        
        .value("_1", Input::KC_1)
        .value("_2", Input::KC_2)

        .value("A", Input::KC_A)
        .value("B", Input::KC_B)
        .value("C", Input::KC_C)
        .value("D", Input::KC_D)
        .value("E", Input::KC_E)
        .value("F", Input::KC_F)
        .value("G", Input::KC_G)
        .value("H", Input::KC_H)
        .value("I", Input::KC_I)
        .value("J", Input::KC_J)
        .value("K", Input::KC_K)
        .value("L", Input::KC_L)
        .value("M", Input::KC_M)
        .value("N", Input::KC_N)
        .value("O", Input::KC_O)
        .value("P", Input::KC_P)
        .value("Q", Input::KC_Q)
        .value("R", Input::KC_R)
        .value("S", Input::KC_S)
        .value("T", Input::KC_T)
        .value("U", Input::KC_U)
        .value("V", Input::KC_V)
        .value("W", Input::KC_W)
        .value("X", Input::KC_X)
        .value("Y", Input::KC_Y)
        .value("Z", Input::KC_Z)

        .value("SPACE", Input::KC_SPACE)

        .value("SUBTRACT", Input::KC_SUBTRACT)
        .value("ADD", Input::KC_ADD)
    ;
    enum_<Input::MouseButtonID>("MouseButtonID")
        .value("LEFT", Input::MB_Left)
        .value("RIGHT", Input::MB_Right)
        .value("MIDDLE", Input::MB_Middle)
        .value("3", Input::MB_Button3)
        .value("4", Input::MB_Button4)
        .value("5", Input::MB_Button5)
        .value("6", Input::MB_Button6)
        .value("7", Input::MB_Button7)
    ;
    class_<Input::MouseEvent, boost::noncopyable>("MouseEvent", no_init)
        .def_readonly("MouseButtonID", &Input::MouseEvent::m_nMouseButtonID)
        .def_readonly("MouseAxisX", &Input::MouseEvent::m_nAxisX)
        .def_readonly("MouseAxisY", &Input::MouseEvent::m_nAxisY)
        .def_readonly("MouseEventID", &Input::MouseEvent::m_nMouseEventID)
    ;
    class_<Input::KeyboardEvent, boost::noncopyable>("KeyboardEvent", no_init)
        .def_readonly("KeyboardEventID", &Input::KeyboardEvent::m_nKeyboardEventID)
        .def_readonly("KeyboardValue", &Input::KeyboardEvent::m_nValue)
    ;
    class_<Input::JoystickEvent, boost::noncopyable>("JoystickEvent", no_init)
        .def_readonly("JoystickIndex", &Input::JoystickEvent::m_nJoystickIndex)
        .def_readonly("JoystickButtonID", &Input::JoystickEvent::m_nJoystickButtonID)
        .def_readonly("JoystickAxis", &Input::JoystickEvent::m_nAxis)
        .def_readonly("JoystickEventID", &Input::JoystickEvent::m_nJoystickEventID)
        .def_readonly("JoystickValue", &Input::JoystickEvent::m_nValue)
    ;

    class_<Input::InputManager_PythonWrap, boost::noncopyable>("InputManager")
        .def("getTaskCapture", &Input::InputManager_PythonWrap::getTaskCapture, return_internal_reference<>())
        .def("initialize", &Input::InputManager_PythonWrap::initialize)
        .def("destroy", &Input::InputManager_PythonWrap::destroy)
        .def("getMouseEvent", &Input::InputManager_PythonWrap::getMouseEvent, return_internal_reference<>())
        .def("getKeyboardEvent", &Input::InputManager_PythonWrap::getKeyboardEvent, return_internal_reference<>())
        .def("getJoystickEvent", &Input::InputManager_PythonWrap::getJoystickEvent, return_internal_reference<>())
    ;

    // Sphere Tree
    class_<Entities::SphereTree_PythonWrapper, boost::noncopyable>("SphereTree")
        .def("addData", &Entities::SphereTree_PythonWrapper::addData)
        .def("setMaxBucketSize", &Entities::SphereTree_PythonWrapper::setMaxBucketSize)
        .def("update", &Entities::SphereTree_PythonWrapper::update)
        .def("debugRender", &Entities::SphereTree_PythonWrapper::debugRender)
        .def("clearDebugRender", &Entities::SphereTree_PythonWrapper::clearDebugRender)
        .def("dump", &Entities::SphereTree_PythonWrapper::dump)
        .def("destroy", &Entities::SphereTree_PythonWrapper::destroy)
    ;

    class_<Entities::SphereData_PythonWrapper, boost::noncopyable>("SphereData")
        .def("getPosition", &Entities::SphereData_PythonWrapper::getPosition)
        .def("setTeam", &Entities::SphereData_PythonWrapper::setTeam)
        .def("setPosition", &Entities::SphereData_PythonWrapper::setPosition)
        .def("getRadius", &Entities::SphereData_PythonWrapper::getRadius)
        .def("setRadius", &Entities::SphereData_PythonWrapper::setRadius)
    ;
}
