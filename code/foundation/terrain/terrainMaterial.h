/*
// Entirity taken from ETL Sample

#ifndef __TERRAINMATERIAL_H__
#define __TERRAINMATERIAL_H__

#include <OgreMaterial.h>
#include <OgreVector2.h>
#include <OgreVector3.h>
#include <OgreMath.h>
#include <ETCommon.h>

namespace Ogre
{
  class Frustum;
  class Technique;
  class SceneNode;
}
namespace ET
{
  class SplattingMaterial;
}


class TerrainMaterial
{
public:
  TerrainMaterial(const Ogre::String& name, Ogre::SceneNode* rootNode);
  ~TerrainMaterial();
  
  void createMaterial();
  void setDecalPosition(const Ogre::Vector3& pos);
  void setDecalSize(const Ogre::Vector2& size);
  void setDecalOrientation(Ogre::Radian orient);
  void setSplattingTextures(const ET::TextureList& textures);
  void setSplattingMaps(const ET::TextureList& maps);
  
  const Ogre::MaterialPtr& getMaterial() const;
  ET::SplattingMaterial* getSplattingMaterial() const;

private:
  void addDecalPass(Ogre::Technique* t);
  
  Ogre::MaterialPtr mBaseMaterial;
  Ogre::Frustum* mDecalFrustum;
  Ogre::SceneNode* mFrustumNode;
  ET::SplattingMaterial* mSplattingMaterial;
};

#endif
*/
