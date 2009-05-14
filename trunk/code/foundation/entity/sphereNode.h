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
            void destroy();

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
            bool canAcceptData(SphereData *_data);

            /**
             */
            SphereNode* getBestFitChild(SphereData *_uData);

            /**
             */
            bool addSphereData(SphereData *_data);

            /**
             */
            void addSphereNode(SphereNode *_node);

            /**
             */
            void dump(int _level);

            /**
             */
            void setMaxBucketSize(unsigned int _size, bool _recursive);

            /**
             */
            void debugRender(const char* _sSceneManagerName, bool _bRecursive, char _nTeamBitfieldFlag, int _nRenderLevel, int _nLevel);

            /**
             */
            void clearDebugRender(const char* _sSceneManagerName);

             

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
            Ogre::String            m_sLineID;
            float                   m_nChildAreaFactor;
            char                    m_nTeamBitfield;
            
            /**
             */
            inline float calcCurrentChildAreaFactor();

            /**
             */
            SphereNode* findFurthestChild();

            /**
             */
            void checkOutsideChildren();

            /**
             */
            void checkOutsideChildren(bool bOverload);

            /**
             */
            void updateToFitChildren();

            /**
             */
            float getTwoClosestChildren(SphereNode *& _node1, SphereNode *& _node2);

            /**
             */
            gmtl::Vec3f determineAverageChildPositions(void);

            /**
             */
            void removeSphereNode(SphereNode *_node);

            /**
             */
            SphereNode* createInternalNode();

            /**
             */
            void updateTeamFlags();
            
        friend class SphereTree;
        };
    };
};
