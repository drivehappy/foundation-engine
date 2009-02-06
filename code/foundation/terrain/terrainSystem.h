#if !defined(__F_TERRAINSYSTEM_H_)
#define __F_TERRAINSYSTEM_H_

// Foundation
#include "graphics/gfxSystem.h"

#include "terrain/terrainMaterial.h"

// ETL
#include <ETFlatTerrain.h>
#include <ETTerrainDescription.h>
#include <ETBrush.h>
#include <ETArray2D.h>
#include <ETDynamicTexture.h>
#include <ETSplattingManager.h>
#include <ETSplattingLayout.h>
#include <ETSplattingMaterial.h>
#include <ETPage.h>


namespace Foundation
{
	namespace Terrain
	{
        class TerrainManager
        {
        public:
            static TerrainManager& getSingleton();

            void create(Ogre::SceneManager *_pSceneMgr);

        protected:
            TerrainManager();
            TerrainManager(const TerrainManager&);
            TerrainManager& operator=(const TerrainManager&);
            ~TerrainManager();

        private:
            
        };
	};
};

#endif	// __F_TERRAINSYSTEM_H_
