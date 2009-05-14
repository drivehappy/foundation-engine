#pragma once

#include "gmtl/gmtl.h"
#include "kernel/kernel.h"

namespace Foundation {
    namespace Entities {
        
        /** 
         */
        class SphereData {
        public:
            SphereData();

            /**
             */
            gmtl::Vec3f getPosition() const;
            /**
             */
            void setPosition(const gmtl::Vec3f _nPosition);

            /**
             */
            float getRadius();
            /**
             */
            void setRadius(const float _nRadius);

            /**
             */
            void setTeam(bool _bBlueTeam);

            /**
             */
            bool getTeam() { return m_bBlueTeam; };

        protected:
            float           m_nRadius;
            gmtl::Vec3f     m_nPosition;
            bool            m_bBlueTeam;
        };

    };
};
