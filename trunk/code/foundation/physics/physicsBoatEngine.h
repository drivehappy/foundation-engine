#if !defined(__F_PHYSICS_BOATENGINE_H_)
#define __F_PHYSICS_BOATENGINE_H_

// Foundation
#include "kernel/kernel.h"
#include "physics/physicsSystem.h"
#include "math/f_math.h"

namespace Foundation
{
    namespace Physics
    {
        class BoatEngine {
        public:
            BoatEngine();
            ~BoatEngine();

            /** Initialize the engine with specific boat values.
                @param
                    The mass of the boat.
                @param
                    Friction encounter on the boat and water (viscosity).
                @param
                    Torque of the rudder.
                @param
                    Torque of the engine rudder.
                @param
                    Force of the keel.
                @param
                    Maximum velocity.
                @param
                    Acceleration of the boat engine with boat values.
                @param
                    Inertia of the boat.
                @param
                    Rotational friction.
                */
            void initialize(float _nMass, float _nFriction, float _nPropellerThrust, float _nRudderTorque, float _nEngineRudderTorque, 
                float _nKeelForce, float _nMaxVelocity, float _nAcceleration, float _nInertia, float _nRotFriction);
            /** Initialize the engine with default boat values.
                */
            void initialize();
            /** Destroy the engine.
                */
            void destroy();

            /** Set the throttle of the engine.
                @param
                    Throttle clamped between -1.0 and 1.0.
                */
            void setThrottle(float _nThrottle)  { m_nThrottle = _nThrottle; };
            /** Set the rudder direction.
                @param
                    Rudder clamped between -1.0 and 1.0.
                */
            void setRudder(float _nRudder);

            /** Get the position.
                @return
                    Vector3 containing the position.
                */
            gmtl::Vec3f getPosition() const;
            /** Get the rotation.
                @return
                    Quaternion containing the rotation.
                */
            gmtl::Quatf getRotation() const;

            /** Set the position.
                @return
                    Vector3 containing the position.
                */
            void setPosition(gmtl::Vec3f _nPosition);
            /** Set the rotation.
                @return
                    Quaternion containing the rotation.
                */
            void setRotation(gmtl::Quatf _nRotation);

            /** Get the rudder value.
                @return
                    Value of the rudder.
                */
            float getRudder();
            /** Get the throttle value.
                @return
                    Value of the throttle.
                */
            float getThrottle();
            /** Get the velocity.
                @return
                    Velocity of the engine.
                */
            gmtl::Vec3f getVelocity() const;
            /** Set the velocity.
                */
            void setVelocity(const gmtl::Vec3f _nVelocity);
            /** Reset the physics to initialization state.
                */
            void reset();

            Task *pTaskUpdate;
            inline static void doTaskUpdate_Wrapper(void *_class, void *_args) {
                reinterpret_cast<BoatEngine*>(_class)->doTaskUpdate(_args);
            };

        private:
            void*               doTaskUpdate(void *_args);

            RigidBody           *m_nPhysicsBody;
            float               m_nMass;
            float               m_nFriction;
            float               m_nInertia;
            float               m_nRotFriction;
            float               m_nThrottle;
            float               m_nRudder;
            float               m_nPropellerThrust;
            float               m_nRudderTorque;
            float               m_nEngineRudderTorque;
            float               m_nKeelForce;
            float               m_nMaxVelocity;
            float               m_nAcceleration;
        };
    };
};

#endif  // __F_PHYSICS_BOATENGINE_H_
