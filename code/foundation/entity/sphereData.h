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

        protected:
            float           m_nRadius;
            gmtl::Vec3f     m_nPosition;
        };

    };
};
