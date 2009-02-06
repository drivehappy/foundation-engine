#if !defined(_PY__F_TERRAINSYSTEM_H_)
#define _PY__F_TERRAINSYSTEM_H_

#include "terrain/terrainSystem.h"

#include "py_graphicManager.h"


namespace Foundation
{
	namespace Terrain
	{
        class TerrainManager_PythonWrapper
        {
        public:
            void create(const char *_sSceneManagerName);
        };
	};
};

#endif	// _PY__F_TERRAINSYSTEM_H_
