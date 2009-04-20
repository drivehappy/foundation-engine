#pragma once

#include "sphereData.h"
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
            SphereNode& addData(const SphereData & _data);

            /** Remove data from the tree.
                @param _iterator Iterator to the data to remove
             */
            void removeData(const SphereTreeIterator & _iterator);

        private:
            
        };      

    };
};
