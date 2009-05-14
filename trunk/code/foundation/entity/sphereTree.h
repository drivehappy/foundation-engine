#pragma once

#include "sphereData.h"
#include "sphereNode.h"

namespace Foundation {
    namespace Entities {
  
        class SphereTree {
        public:
            SphereTree();
            ~SphereTree();

            void destroy();

            /** Add new data to the tree.
                @param _data Data to add
                @return Node in which data resides
             */
            vector<SphereData *>::iterator& addData(SphereData *_data);

            /** Remove data from the tree.
                @param _iterator Iterator to the data to remove
             */
            void removeData(vector<SphereData *>::iterator& _iterator);

            /**
             */
            unsigned int getChildCount();

            /**
             */
            unsigned int getMaxDepth();

            /**
             */
            void setMaxBucketSize(unsigned int _size);

            /**
             */
            void update();

            /**
             */
            void dump();

            /**
             */
            void debugRender(const char* _sSceneManagerName, int _nRenderLevel);

            /**
             */
            void clearDebugRender(const char* _sSceneManagerName);

        private:
            SphereNode             *m_pRoot;
            unsigned int            m_nMinRadius, m_nMaxRadius;
            vector<SphereData>      m_uDataStore;
            unsigned int            m_nDebugDataCount;
        };      

    };
};
