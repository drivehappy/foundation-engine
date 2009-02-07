#include "terrainSystem.h"

using namespace Foundation;
using namespace Foundation::Terrain;

using namespace ET;


TerrainManager& TerrainManager::getSingleton()
{
    static TerrainManager m_Singleton;

    return m_Singleton;
}

void TerrainManager::create(Ogre::SceneManager *_pSceneMgr)
{
    size_t width = 257;
    size_t height = 257;
    Ogre::Vector2 grainSize(20, 20);
    ET::RealArray2D heights(width, height, 0.5f);
    const char mTexture[] = "jungle_0.jpg";

    m_pSceneMgr = _pSceneMgr;

    Ogre::SceneNode *mTerrainNode = _pSceneMgr->getRootSceneNode()->createChildSceneNode("TerrainSystem_Node");
    TerrainMaterial *mMaterial = new TerrainMaterial("TerrainMaterial", mTerrainNode);
    ET::ArrayBrush *mBrush = new ET::ArrayBrush(ET::RealArray2D(2,2), grainSize);

    ET::FlatTerrain *mTerrain = new ET::FlatTerrain(heights, grainSize, 20);

    ET::DynamicTexture *mColourMap = new ET::DynamicTexture("ETColourMap", "General", heights.sizeX()-1,
        heights.sizeY()-1, Ogre::PF_BYTE_RGB, grainSize);

    ET::DynamicTexture *mLightMap = new ET::DynamicTexture("ETLightMap", "General", 128, 128,
        Ogre::PF_BYTE_RGB, grainSize);

    ET::SplattingManager *mSplattingManager = new ET::SplattingManager("ETSplattingMap", "General",
        128, 128, Ogre::PF_BYTE_RGB, grainSize, mTexture);

    ET::Page* page = new ET::Page("ETTerrain", mTerrain, 33, 33, true, 1);

    mMaterial->createMaterial();

    mSplattingManager->addListener(mMaterial->getSplattingMaterial());

    page->setMaterial(mMaterial->getMaterial());

    page->setQueryFlags(TERRAIN_MASK);
    mTerrainNode->attachObject(page);
    mTerrainNode->setPosition((width * grainSize[0]) / -2.0, 0, (height * grainSize[1]) / -2.0);
    mTerrainNode->showBoundingBox(true);

    //try some splat
    mTerrain->deform(50, 50, mBrush, 10);
    mTerrain->update();
                           
    //and deform
    mSplattingManager->splat(50, 50, mBrush, "jungle_2.jpg", 10);
    mSplattingManager->getSplattingLayout()->update();
}

gmtl::Vec3f TerrainManager::getRayIntersection(const char *sSceneManagerName, const char *sCameraName, gmtl::Vec2f _nWorldPoint, gmtl::Vec2f _nScreenWidth)
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
    // DEBUG
    f_printf("--> Result Size: %i\n", uResult.size());
    for (itrResult = uResult.begin(); itrResult != uResult.end(); itrResult++) {
        Foundation::Graphic::SelectionObject *pObject = (Foundation::Graphic::SelectionObject*)(itrResult->movable);
        f_printf("--  Name: %s\n", pObject->getName());
    }
    //--

    for (itrResult = uResult.begin(); itrResult != uResult.end(); itrResult++) {
        Ogre::Vector3 nIntersectionPoint = uRayIntersection.getPoint(itrResult->distance);
        
        return gmtl::Vec3f(nIntersectionPoint[0], nIntersectionPoint[1], nIntersectionPoint[2]);
    }

    f_printf("[TerrainSystem] Warning: Ray Intersection didn't hit terrain.\n");
    return gmtl::Vec3f(0, 0, 0);
}

TerrainManager::TerrainManager()
{
}

TerrainManager::~TerrainManager()
{
}
