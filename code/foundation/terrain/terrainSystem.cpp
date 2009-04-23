#include "terrainSystem.h"

using namespace Foundation;
using namespace Foundation::Terrain;

using namespace ET;


Terrain::TerrainManager& Terrain::TerrainManager::getSingleton()
{
    static Terrain::TerrainManager m_Singleton;

    return m_Singleton;
}

void Terrain::TerrainManager::create(Ogre::SceneManager *_pSceneMgr, Ogre::Camera *_pCamera)
{
    m_pSceneMgr = _pSceneMgr;

    // create terrain manager
    mTerrainMgr = new ET::TerrainManager(_pSceneMgr);
    mTerrainMgr->setLODErrorMargin(1, _pCamera->getViewport()->getActualHeight());
    mTerrainMgr->setUseLODMorphing(true, 0.1, "morphFactor");

    // create a fresh, mid-high terrain for editing
    ET::TerrainInfo terrainInfo(129, 129, vector<float>(129*129, 0.5f));
    // set position and size of the terrain
    terrainInfo.setExtents(Ogre::AxisAlignedBox(-750, 0, -750, 750, 0, 750));
    // now render it
    mTerrainMgr->createTerrain(terrainInfo);

    // create the splatting manager
    mSplatMgr = new ET::SplattingManager("ETSplatting", "ET", 128, 128, 3);
    // specify number of splatting textures we need to handle
    mSplatMgr->setNumTextures(6);

    // create a manual lightmap texture
    Ogre::TexturePtr lightmapTex = Ogre::TextureManager::getSingleton().createManual(
    "ETLightmap", "ET", Ogre::TEX_TYPE_2D, 128, 128, 1, Ogre::PF_BYTE_RGB);
    Ogre::Image lightmap;
    ET::createTerrainLightmap(terrainInfo, lightmap, 128, 128, Ogre::Vector3(1, -1, 1), Ogre::ColourValue::White,
    Ogre::ColourValue(0.3, 0.3, 0.3));
    lightmapTex->getBuffer(0, 0)->blitFromMemory(lightmap.getPixelBox(0, 0));

    // load the terrain material and assign it
    Ogre::MaterialPtr material (Ogre::MaterialManager::getSingleton().getByName("ETTerrainMaterial"));
    mTerrainMgr->setMaterial(material);
}

gmtl::Vec3f Terrain::TerrainManager::getRayIntersection(const char *sSceneManagerName, const char *sCameraName, gmtl::Vec2f _nWorldPoint, gmtl::Vec2f _nScreenWidth)
{
    Ogre::Camera*                       pCamera;
    Ogre::Ray                           uRayIntersection;
    Ogre::RaySceneQuery*                uRaySceneQuery;
    Ogre::RaySceneQueryResult           uResult;
    Ogre::RaySceneQueryResult::iterator itrResult;
    float                               nX, nY;

    if (!m_pSceneMgr->hasCamera(sCameraName)) {
        f_printf("[TerrainManager] Error: Camera %s not found.\n", sCameraName);
        return gmtl::Vec3f(0, 0, 0);
    }

    pCamera = m_pSceneMgr->getCamera(sCameraName);
    
    f_printf("[TerrainManager] DEBUG: Getting Ray Intersection (Screen = %f, %f), (Point = %f, %f)\n", _nScreenWidth[0], _nScreenWidth[1], _nWorldPoint[0], _nWorldPoint[1]);
    
    nX = _nWorldPoint[0] / _nScreenWidth[0];
    nY = _nWorldPoint[1] / _nScreenWidth[1];

    uRayIntersection = pCamera->getCameraToViewportRay(nX, nY);
    uRaySceneQuery = m_pSceneMgr->createRayQuery(uRayIntersection);
    uRaySceneQuery->setRay(uRayIntersection);
    uRaySceneQuery->setSortByDistance(true);
    uRaySceneQuery->setQueryMask(TERRAIN_MASK);
    uRaySceneQuery->setQueryTypeMask(Ogre::SceneManager::WORLD_GEOMETRY_TYPE_MASK);

    uResult = uRaySceneQuery->execute();

    for (itrResult = uResult.begin(); itrResult != uResult.end(); itrResult++) {
        Ogre::Vector3 nIntersectionPoint = uRayIntersection.getPoint(itrResult->distance);
        
        return gmtl::Vec3f(nIntersectionPoint[0], nIntersectionPoint[1], nIntersectionPoint[2]);
    }

    f_printf("[TerrainSystem] Warning: Ray Intersection didn't hit terrain.\n");
    return gmtl::Vec3f(0, 0, 0);
}

Terrain::TerrainManager::TerrainManager()
{
}

Terrain::TerrainManager::~TerrainManager()
{
}
