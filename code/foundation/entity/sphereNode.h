#pragma once

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

        // Constructor Guard
        protected:
            /** Default Constructor.
             */
            SphereNode();
            

        private:
            vector<SphereNode> m_uChildren;
        };
    };
};
