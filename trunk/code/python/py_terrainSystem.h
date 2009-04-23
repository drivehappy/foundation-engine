#if !defined(_PY__F_TERRAINSYSTEM_H_)
#define _PY__F_TERRAINSYSTEM_H_

#include "terrain/terrainSystem.h"

#include "py_graphicManager.h"
#include "py_gmtl.h"


namespace Foundation
{
	namespace Terrain
	{
        class TerrainManager_PythonWrapper
        {
        public:
            void create(const char *_sSceneManagerName, const char *_sCameraName);

            gmtlVector3_PythonWrapper getRayIntersection(const char *sSceneManagerName, const char *sCameraName, gmtlVector2_PythonWrapper _nWorldRect, gmtlVector2_PythonWrapper _nScreenWidth);
        };
	};
};

#endif	// _PY__F_TERRAINSYSTEM_H_
