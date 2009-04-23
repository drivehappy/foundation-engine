#if !defined(__F_TERRAINSYSTEM_H_)
#define __F_TERRAINSYSTEM_H_

// Foundation
#include "graphics/gfxSystem.h"

//#include "terrain/terrainMaterial.h"

// ETL
/*
#include <ETFlatTerrain.h>
#include <ETTerrainDescription.h>
#include <ETBrush.h>
#include <ETArray2D.h>
#include <ETDynamicTexture.h>
#include <ETSplattingManager.h>
#include <ETSplattingLayout.h>
#include <ETSplattingMaterial.h>
#include <ETPage.h>
*/

// ETM
#include "ETTerrainManager.h"
#include "ETTerrainInfo.h"
#include "ETBrush.h"
#include "ETSplattingManager.h"


namespace Foundation
{
	namespace Terrain
	{
        static Ogre::uint32 TERRAIN_MASK = 0x2;

        class TerrainManager
        {
        public:
            static TerrainManager& getSingleton();

            void create(Ogre::SceneManager *_pSceneMgr, Ogre::Camera *_pCamera);

            gmtl::Vec3f getRayIntersection(const char *sSceneManagerName, const char *sCameraName, gmtl::Vec2f _nWorldRect, gmtl::Vec2f _nScreenWidth);

        protected:
            TerrainManager();
            TerrainManager(const TerrainManager&);
            TerrainManager& operator=(const TerrainManager&);
            ~TerrainManager();

        private:
            ET::TerrainManager      *mTerrainMgr;
            ET::SplattingManager    *mSplatMgr;
            Ogre::SceneManager      *m_pSceneMgr;
        };
	};
};

#endif	// __F_TERRAINSYSTEM_H_
