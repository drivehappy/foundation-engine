#pragma once

#include "sphereData.h"

#include "gmtl/gmtl.h"

// STL Includes
#include <vector>
using std::vector;


namespace Foundation {
    namespace Entity {
        
        class SphereTree;

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
            void setPosition(gmtl::Vec3f _nPosition);

            /**
             */
            float getRadius() const;

            /**
             */
            unsigned int getChildCount();

            /**
             */
            unsigned int getMaxDepth();

            /**
             */
            void update();

            /**
             */
            SphereNode* getBestFitNode(const SphereData & _uData);

            /**
             */
            void debugRender();

        // Constructor Guard
        protected:
            /** Default Constructor.
             */
            SphereNode();

            /**
             */
            SphereNode(unsigned int _nMaxBucketSize);
            

        private:
            vector<SphereNode *>    m_uChildren;
            gmtl::Vec3f             m_nPosition;
            float                   m_nRadius;
            float                   m_nElasticity;
            unsigned int            m_nMaxBucketSize;

            /**
             */
            void updateToFitChildren(gmtl::Vec3f & _nPosition, float & _nRadius);
            
        friend class SphereTree;
        };
    };
};
