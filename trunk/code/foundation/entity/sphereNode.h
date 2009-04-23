#pragma once

#include "gmtl/gmtl.h"

// STL Includes
#include <vector>
using std::vector;


namespace Foundation {
    namespace Entity {
        
        class SphereNode {
        public:
            /** Destructor.
             */
            ~SphereNode();

            /**
             */
            gmtl::Vec3f getPosition();

            /**
             */
            unsigned int getChildCount();

            /**
             */
            unsigned int getMaxDepth();

            /**
             */
            void debugRender();

        // Constructor Guard
        protected:
            /** Default Constructor.
             */
            SphereNode();
            

        private:
            vector<SphereNode *>    m_uChildren;
            gmtl::Vec3f             m_nPosition;
            float                   m_nRadius;
            float                   m_nElasticity;
            float                   m_nMaxRadius;

            /**
             */
            void updateToFitChildren(gmtl::Vec3f & _nPosition, float & _nRadius);
            

        };
    };
};
