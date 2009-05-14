#pragma once

// Boost
#include <boost/python.hpp>
using namespace boost::python;

// Foundation
#include "foundation.h"
#include "entity/sphereData.h"
#include "entity/sphereNode.h"
#include "entity/sphereTree.h"

#include "py_gmtl.h"

namespace Foundation
{
	namespace Entities
	{
        class SphereDataIterator_PythonWrapper : public vector<SphereData *>::iterator
        {
            SphereDataIterator_PythonWrapper();
        };

        class SphereData_PythonWrapper : public SphereData
        {
        public:
            gmtlVector3_PythonWrapper getPosition();

            void setPosition(const gmtlVector3_PythonWrapper _nPosition);
        };

		class SphereTree_PythonWrapper : public SphereTree
		{
		public:
			SphereTree_PythonWrapper();

			void addData(SphereData_PythonWrapper *_uData);

            void debugRender(const char* _sSceneManagerName, int _nRenderLevel);
		};
	}
};
