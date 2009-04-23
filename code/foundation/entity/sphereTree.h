#pragma once

#include "sphereData.h"
#include "sphereNode.h"
#include "sphereTreeIterator.h"

namespace Foundation {
    namespace Entity {
  
        class SphereTree {
        public:
            SphereTree();
            ~SphereTree();

            /** Add new data to the tree.
                @param _data Data to add
                @return Node in which data resides
             */
            void addData(const SphereData & _data);

            /** Remove data from the tree.
                @param _iterator Iterator to the data to remove
             */
            void removeData(SphereNode* _iterator);

            /**
             */
            unsigned int getChildCount();

            /**
             */
            unsigned int getMaxDepth();

        private:
            SphereNode *m_pRoot;
            unsigned int m_nMinRadius, m_nMaxRadius;
        };      

    };
};
