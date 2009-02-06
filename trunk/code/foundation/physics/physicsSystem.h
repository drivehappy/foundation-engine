#if !defined(__F_PHYSICSSYSTEM_H_)
#define __F_PHYSICSSYSTEM_H_

// Foundation
#include "kernel/kernel.h"

// Bullet Physics
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/NarrowPhaseCollision/btGjkPairDetector.h"
#include "BulletCollision/NarrowPhaseCollision/btPointCollector.h"
#include "BulletCollision/NarrowPhaseCollision/btVoronoiSimplexSolver.h"
#include "BulletCollision/CollisionDispatch/btBoxBoxCollisionAlgorithm.h"

namespace Foundation {

    namespace Physics {

        /**
         */
        class CollisionEvent {
        public:
            CollisionEvent(btPersistentManifold *_uContactManifold);
            ~CollisionEvent();

            unsigned int getBody0();
            unsigned int getBody1();

            gmtl::Vec3f getContactPoint();

        private:
            btPersistentManifold    *m_uContactManifold;
        };

        /** Class that provides abstracted values from the physics system to render
            bounding boxes for debugging in a graphical environment.
         */
        struct PhysicsDebugObject {
            gmtl::Vec3f     m_nContactPoint;
            gmtl::Point2f   m_nPosition;
            gmtl::Vec3f     m_nAABBMin;
            gmtl::Vec3f     m_nAABBMax;
            void            *m_pShape;  // Hold a pointer to the true shape so 
                                        // we know what object to remove from list
        };

        /** Class that provides a rigidbody definition.
         */
        class RigidBody {
            public:
                RigidBody();

                void update(float fElapsedTime);
                void reset();

                void applyForce(const gmtl::Vec3f _nForce);
                void applyAlignedForce(const gmtl::Vec3f _nForce);
                void applyAngularForce(const gmtl::Vec3f _nForce);
                void applyAlignedAngularForce(const gmtl::Vec3f _nForce);

                inline gmtl::Vec3f getPosition() const { return m_nPosition; };
                inline gmtl::Quatf getRotation() const { return m_nRotation; };
                inline gmtl::Vec3f getVelocity() const { return m_nVelocity; };
                inline float getAngularVelocity() const { return m_nAngularVelocity; };
                inline gmtl::Vec3f getAngularForce() const { return m_nAngularForce; };

                inline void setPosition(gmtl::Vec3f _nPosition) { m_nPosition = _nPosition; };
                inline void setRotation(gmtl::Quatf _nRotation) { m_nRotation = _nRotation; };
                inline void setMass(float _nMass)               { m_nMass = _nMass; };
                inline void setInertia(float _nInertia)         { m_nInertia = _nInertia; };
                inline void setFriction(float _nFriction)       { m_nFriction = _nFriction; };
                inline void setRotFriction(float _nRotFriction) { m_nRotFriction = _nRotFriction; };
                inline void setVelocity(gmtl::Vec3f _nVelocity) { m_nVelocity = _nVelocity; };

            public: // private:
                gmtl::Vec3f         m_nPosition;
                gmtl::Vec3f         m_nVelocity;
                gmtl::Vec3f         m_nForce;
                gmtl::Quatf         m_nRotation;
                float               m_nAngularVelocity;
                gmtl::Vec3f         m_nAngularForce;
                gmtl::Vec3f         m_nAlignedAngularForce;
                //gmtl::Vec3f         m_nVelocityDecay;
                float               m_nMass;
                float               m_nInertia;
                float               m_nFriction;
                float               m_nRotFriction;
        };

        /**
         */
        enum CollisionShapeId {
            BOX = 0x0,
            SPHERE,
            PLANE,
        };

        /** Provides rigid body physics to the engine.
            Acts as a factory for RigidBody's and CollisionObjectBox's.
         */
        class PhysicsManager {
            public:
                /** Get our singleton class.
                 */
                static PhysicsManager& getSingleton();

                /** Initialize the physics system.
                 */
                static void initialize();
                /** Destroy the phyics system.
                 */
                static void destroy();

                /** Create a new rigidbody.
                 */
                static RigidBody* createRigidBody();
                /** Destroy a created rigidbody.
                 */
                static void destroyRigidBody(RigidBody *_pBody);

                /**
                 */
                static unsigned int createCollisionShape(CollisionShapeId _uShape, gmtl::Vec3f _nSize);
                /**
                 */
                static void destroyCollisionObject(const unsigned int _nId);
                /**
                 */
                static void setCollisionShapeOrigin(unsigned int _nId, gmtl::Vec3f _nOrigin);
                /**
                 */
                static gmtl::Vec3f getCollisionShapeOrigin(unsigned int _nId);
                /**
                 */
                static void setCollisionShapeRotation(unsigned int _nId, gmtl::Quatf _nRotation);
                /**
                 */
                static void setCollisionShapeRotation(unsigned int _nId, gmtl::Vec3f _nAxis, float _nAngle);

                //static bool CollisionCallback(btManifoldPoint & cp, const btCollisionObject *pObject1, int partId0, int index0, const btCollisionObject *pObject2, int partId1, int index1);

                /**
                 */
                static unsigned int getCollisionObjectId(btCollisionObject *_pObject);
                /**
                 */
                static btCollisionObject* getCollisionObject(unsigned int _nId);

                /**
                 */
                static CollisionEvent* consumeCollisionEvent();
                /**
                 */
                static unsigned int getCollisionEventCount();
                /** Destroys a collision event. This is needed because Python utilizes our events when it consumes them,
                    therfore we don't know when to cleanup the memory, so this must be explicitly called.
                 */
                static void destroyCollisionEvent(CollisionEvent *_pEvent);

                /** Return a vector of PhysicDebugObject's for graphics rendering.
                 */
                inline vector<PhysicsDebugObject*> getDebugObjects() {
                    return m_PhysicsDebugObjectList;
                }

                static Task *pTaskUpdate;
                inline static void doTaskUpdate_Wrapper(void *_class, void *_args) {
                    reinterpret_cast<PhysicsManager*>(_class)->doTaskUpdate(_args);
                };

            protected:
                PhysicsManager();
                PhysicsManager(const PhysicsManager&);
                PhysicsManager& operator=(const PhysicsManager&);
                ~PhysicsManager();

            private:
                void* doTaskUpdate(void *_args);

                static vector<PhysicsDebugObject*>              m_PhysicsDebugObjectList;
                static vector<RigidBody*>                       m_RigidBodyList;

                // Collision members
                static btDefaultCollisionConfiguration          *collisionConfiguration;
                static btCollisionDispatcher                    *dispatcher;
                //static btAxisSweep3                             *broadphase;
                static btDbvtBroadphase                         *broadphase;
                static btCollisionWorld                         *collisionWorld;
                static map<unsigned int, btCollisionObject*>    collisionObjects;    // Use the memory location as an ID for now
                static vector<CollisionEvent *>                 m_uCollisionEventList;
                static vector<btCollisionShape *>               m_uCollisionShapeList;
        };

    };

};

#endif  // __F_PHYSICSSYSTEM_H_
