// Taken from ETL Sample

#include "TerrainMaterial.h"
#include <OgreMaterialManager.h>
#include <OgreTechnique.h>
#include <OgrePass.h>
#include <OgreTextureUnitState.h>
#include <OgreFrustum.h>
#include <OgreSceneNode.h>
#include <OgreMaterialSerializer.h>
#include <ETSplattingMaterial.h>

TerrainMaterial::TerrainMaterial(const Ogre::String& name, Ogre::SceneNode* rootNode)
{
    mBaseMaterial = Ogre::MaterialPtr (Ogre::MaterialManager::getSingleton().create(name+"/Base", "General"));
    mDecalFrustum = new Ogre::Frustum();
    mFrustumNode = rootNode->createChildSceneNode("DecalFrustumNode");
    mFrustumNode->attachObject(mDecalFrustum);
    mDecalFrustum->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
    mFrustumNode->setOrientation(Ogre::Quaternion(Ogre::Degree(90), Ogre::Vector3::UNIT_X));
    mSplattingMaterial = new ET::SplattingMaterial(name, "General", Ogre::PF_BYTE_RGB);
    //mSplattingMaterial->setUseVertexMorphing(true);
    mSplattingMaterial->setBaseMaterial(mBaseMaterial);
    mSplattingMaterial->setTextureRepeat(1, 1);
}

TerrainMaterial::~TerrainMaterial()
{
    Ogre::MaterialSerializer serializer;
    serializer.exportMaterial(mSplattingMaterial->getMaterial(), "SplattingMaterial.material", true);
    delete mSplattingMaterial;
    Ogre::MaterialManager::getSingleton().remove(mBaseMaterial->getName());
    mFrustumNode->getParentSceneNode()->removeAndDestroyChild("DecalFrustumNode");
    delete mDecalFrustum;
}

void TerrainMaterial::createMaterial()
{
    mBaseMaterial->removeAllTechniques();
    Ogre::Technique* t = mBaseMaterial->createTechnique();

    Ogre::Pass* p = t->createPass();
    p->setSceneBlending(Ogre::SBT_MODULATE);
    p->setVertexProgram("ET/Programs/VSLodMorph2");
    p->setFragmentProgram("ET/Programs/PSColour");
    p->createTextureUnitState("ETColourMap");  
    p->createTextureUnitState("ETLightMap");

    //addDecalPass(t);

    mBaseMaterial->setLightingEnabled(true);

    mSplattingMaterial->createMaterial(); 
}

const Ogre::MaterialPtr& TerrainMaterial::getMaterial() const
{
    return mSplattingMaterial->getMaterial();
}

void TerrainMaterial::setSplattingMaps(const ET::TextureList& maps)
{
    mSplattingMaterial->setSplattingMaps(maps);
}

void TerrainMaterial::setSplattingTextures(const ET::TextureList& textures)
{
    mSplattingMaterial->setSplattingTextures(textures);
}


void TerrainMaterial::setDecalPosition(const Ogre::Vector3& pos)
{
  mFrustumNode->setPosition(pos);
}


void TerrainMaterial::setDecalSize(const Ogre::Vector2& size)
{
  mDecalFrustum->setFOVy(Ogre::Degree(45));
  mDecalFrustum->setNearClipDistance(size.y);
#if OGRE_VERSION_MAJOR == 1 && OGRE_VERSION_MINOR <= 4
  mDecalFrustum->setAspectRatio(size.x / size.y);
#else
  mDecalFrustum->setOrthoWindow(size.x, size.y);
#endif
}


void TerrainMaterial::setDecalOrientation(Ogre::Radian orient)
{
  Ogre::Quaternion quat1 (Ogre::Degree(90), Ogre::Vector3::UNIT_X);
  Ogre::Quaternion quat2 (orient, Ogre::Vector3::UNIT_Z);
  mFrustumNode->setOrientation(quat1 * quat2);
}



void TerrainMaterial::addDecalPass(Ogre::Technique* t)
{
  Ogre::Pass* p = t->createPass();
  p->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
  p->setDepthBias(2.5, 2.5);

  // create decal texture unit state
  Ogre::TextureUnitState* texState = p->createTextureUnitState("decal.png");
  texState->setProjectiveTexturing(true, mDecalFrustum);
  texState->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
  texState->setTextureFiltering(Ogre::FO_POINT, Ogre::FO_LINEAR, Ogre::FO_NONE);
    
}

ET::SplattingMaterial* TerrainMaterial::getSplattingMaterial() const
{
  return mSplattingMaterial;
}

