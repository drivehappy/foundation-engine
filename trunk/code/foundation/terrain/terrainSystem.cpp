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
    Ogre::Vector2 grainSize(10, 10);
    ET::RealArray2D heights(width, height, 0.5f);
    const char mTexture[] = "jungle_0.jpg";


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

    mTerrainNode->attachObject(page);
    mTerrainNode->setPosition(-(width-1.0), 0, -(height-1.0));

    //try some splat
    mTerrain->deform(50, 50, mBrush, 10);
    mTerrain->update();
                           
    //and deform
    mSplattingManager->splat(50, 50, mBrush, "jungle_2.jpg", 10);
    mSplattingManager->getSplattingLayout()->update();
}

TerrainManager::TerrainManager()
{
}

TerrainManager::~TerrainManager()
{
}
