#if !defined(_PY__F_PHYSICSSYSTEM_H_)
#define _PY__F_PHYSICSSYSTEM_H_

// Boost
#include <boost/python.hpp>
using namespace boost::python;

// Foundation
#include "foundation.h"

#include "py_gmtl.h"

namespace Foundation {

    namespace Physics {

        /** Class that provides a Python interface to the BoatEngine
         */
        class BoatEngine_PythonWrapper : public BoatEngine, public boost::python::pickle_suite {
        public:
            BoatEngine_PythonWrapper() {};
            ~BoatEngine_PythonWrapper() {};

            gmtlVector3_PythonWrapper getPosition() const;
            gmtlQuaternion_PythonWrapper getRotation() const;

            void setPosition(gmtlVector3_PythonWrapper _nPosition);
            void setRotation(gmtlQuaternion_PythonWrapper _nRotation);

            gmtlVector3_PythonWrapper getVelocity() const;
            void setVelocity(const gmtlVector3_PythonWrapper _nVelocity);

            /** Return our wrapped task.
            */
            Task* getTaskUpdate() const;
        };

        /**
         */
        struct boatengine_pickle_suite : pickle_suite {
            static tuple getinitargs(const BoatEngine_PythonWrapper& boatEngine);
            static tuple getstate(const BoatEngine_PythonWrapper& boatEngine);
            static void setstate(BoatEngine_PythonWrapper& boatEngine, tuple state);
        };

        /** Class that provides a Python interface to the RigidBody
         */
        class RigidBody_PythonWrapper : public RigidBody {
            public:
                RigidBody_PythonWrapper() {};
                ~RigidBody_PythonWrapper() {};

                void update(float fElapsedTime);

                void applyForce(const gmtlVector3_PythonWrapper _nForce);
                void applyAlignedForce(const gmtlVector3_PythonWrapper _nForce);
                void applyAngularForce(const gmtlVector3_PythonWrapper _nForce);
                void applyAlignedAngularForce(const gmtlVector3_PythonWrapper _nForce);

                gmtlVector3_PythonWrapper getPosition() const;
                gmtlQuaternion_PythonWrapper getRotation() const;
        };

        /**
         */
        class CollisionEvent_PythonWrapper : public CollisionEvent {
        public:
            unsigned int getBody0();
            unsigned int getBody1();

            gmtlVector3_PythonWrapper getContactPoint();
        };

        /** Class that provides a Python interface to the PhysicsManager
         */
        class PhysicsManager_PythonWrap {
            public:
                PhysicsManager_PythonWrap();
                ~PhysicsManager_PythonWrap();

                /** Return our wrapped task.
                */
                Task *getTaskUpdate() const;

                /** Initialize the physics system.
                */
                void initialize();
                /** Destroy the phyics system.
                */
                void destroy();

                /** Create a new rigidbody.
                */
                RigidBody_PythonWrapper* createRigidBody();
                /** Destroy a created rigidbody.
                */
                void destroyRigidBody(RigidBody_PythonWrapper *_pBody);

                /**
                 */
                unsigned int createCollisionShape(CollisionShapeId _uShape, gmtlVector3_PythonWrapper _nSize);
                /**
                 */
                void destroyCollisionObject(const unsigned int _nId);
                /**
                 */
                void setCollisionShapeOrigin(unsigned int _nId, gmtlVector3_PythonWrapper _nOrigin);
                /**
                 */
                gmtlVector3_PythonWrapper getCollisionShapeOrigin(unsigned int _nId);
                /**
                 */
                void setCollisionShapeRotation(unsigned int _nId, gmtlQuaternion_PythonWrapper _nRotation);
                /**
                 */
                void setCollisionShapeRotation(unsigned int _nId, gmtlVector3_PythonWrapper _nAxis, float _nAngle);

                /**
                 */
                CollisionEvent_PythonWrapper* consumeCollisionEvent();
                /**
                 */
                unsigned int getCollisionEventCount();
                /** Destroys a collision event. This is needed because Python utilizes our events when it consumes them,
                    therfore we don't know when to cleanup the memory, so this must be explicitly called.
                 */
                void destroyCollisionEvent(CollisionEvent_PythonWrapper *_pEvent);

                /** Return a vector of PhysicDebugObject's for graphics rendering.
                */ 
                /*
                inline vector<PhysicsDebugObject*> getDebugObjects() {
                    return PhysicsManager::getSingleton().getDebugObjects();
                }
                */
    
        };

    };

};

#endif  // _PY__F_PHYSICSSYSTEM_H_
