#include "py_terrainSystem.h"

using namespace Foundation::Terrain;
using namespace Foundation::Graphic;


void TerrainManager_PythonWrapper::create(const char *_sSceneManagerName)
{
    SceneManager *pSceneMgr = GraphicManager::getSingleton().getSceneManager(_sSceneManagerName);

    if (pSceneMgr) {
        TerrainManager::getSingleton().create(pSceneMgr);
    } else {
        f_printf("Warning: SceneManager %s not found.\n", _sSceneManagerName);
    }
}
