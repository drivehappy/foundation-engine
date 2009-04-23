#pragma once

#include "gmtl/gmtl.h"

namespace Foundation {
    namespace Entity {
        
        /** Abstract class that grabs data positions.
         */
        class SphereData {
        public:
            /** Pure virtual function for derived classes to implement returning position of their data.
                @return Position
             */
            gmtl::Vec3f virtual getPosition(void) = 0;

            /**
             */
            float virtual getRadius(void) = 0;
        };

    };
};
