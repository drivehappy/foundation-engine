#pragma once

#include "sphereData.h"

#include "graphics/gfxSystem.h"
#include "gmtl/gmtl.h"

// STL Includes
#include <vector>
using std::vector;


namespace Foundation {
    namespace Entities {
        
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
            SphereNode* getBestFitNode(SphereData* _uItr);

            /**
             */
            void debugRender(const char* _sSceneManagerName);

             

        // Constructor Guard
        protected:
            /** Default Constructor.
             */
            SphereNode();

            /**
             */
            SphereNode(bool _bDataNode, unsigned int _nMaxBucketSize = 1);
            

        private:
            bool                    m_bDataNode;
            vector<SphereNode *>    m_uNodeChildren;
            SphereData             *m_pData;
            gmtl::Vec3f             m_nPosition;
            float                   m_nRadius;
            float                   m_nElasticity;
            unsigned int            m_nMaxBucketSize;
            char                   *m_sGraphicID;

            /**
             */
            void updateToFitChildren(gmtl::Vec3f & _nPosition, float & _nRadius);
            
        friend class SphereTree;
        };
    };
};
