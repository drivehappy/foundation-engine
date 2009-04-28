#pragma once

#include "gmtl/gmtl.h"

namespace Foundation {
    namespace Entities {
        
        /** 
         */
        class SphereData {
        public:
            SphereData();

            /** Pure virtual function for derived classes to implement returning position of their data.
                @return Position
             */
            gmtl::Vec3f getPosition();

            /**
             */
            float getRadius();

        protected:
            float           m_nRadius;
            gmtl::Vec3f     m_nPosition;
        };

    };
};
