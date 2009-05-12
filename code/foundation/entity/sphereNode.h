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
            void setRadius(float _nRadius);

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
            bool addSphereData(SphereData *_data);

            /**
             */
            void addSphereNode(SphereNode *_node);

            /**
             */
            void debugRender(const char* _sSceneManagerName, bool _bRecursive);

             

        // Constructor Guard
        protected:
            /** Default Constructor.
             */
            SphereNode(const float _nMinRadius, const float _nMaxRadius);

            /**
             */
            SphereNode(const float _nMinRadius, const float _nMaxRadius, SphereData *_pData);

            /**
             */
            SphereNode(const float _nMinRadius, const float _nMaxRadius, bool _bDataNode, unsigned int _nMaxBucketSize = 1);
            
            bool                    m_bRootNode;
            SphereNode             *m_pParentNode;

        private:
            bool                    m_bDataNode;
            vector<SphereNode *>    m_uNodeChildren;
            SphereData             *m_pData;
            gmtl::Vec3f             m_nPosition;
            float                   m_nRadius;
            float                   m_nMinRadius, m_nMaxRadius;
            float                   m_nElasticity;
            unsigned int            m_nMaxBucketSize;
            char                   *m_sGraphicID;
            

            /**
             */
            void updateToFitChildren();

            /**
             */
            float getTwoClosestChildren(SphereNode *& _node1, SphereNode *& _node2);
            
        friend class SphereTree;
        };
    };
};
