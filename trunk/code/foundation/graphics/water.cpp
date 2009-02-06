#include "water.h"

// S_ENABLE_GRAPHIC is defined in premake.lua, it should only be altered there
// to allow for easy stripping/adding of libraries actually needed to be compiled
#if defined(S_ENABLE_GRAPHIC)

using namespace Foundation;
using namespace Foundation::Graphic;


/**********************************************************************
  Static declarations
**********************************************************************/
// Lights
#define NUM_LIGHTS 1

// the light
Ogre::Light* mLights[NUM_LIGHTS];
// billboards for lights
Ogre::BillboardSet* mLightFlareSets[NUM_LIGHTS];
Ogre::Billboard* mLightFlares[NUM_LIGHTS];
// Positions for lights
Ogre::Vector3 mLightPositions[NUM_LIGHTS] =
{
    Ogre::Vector3(00, 1000, 00)
};
// Base orientations of the lights
Ogre::Real mLightRotationAngles[NUM_LIGHTS] = { 35 };
Ogre::Vector3 mLightRotationAxes[NUM_LIGHTS] = {
    Ogre::Vector3::UNIT_X
};
// Rotation speed for lights, degrees per second
Ogre::Real mLightSpeeds[NUM_LIGHTS] = { 30};

// Colours for the lights
Ogre::ColourValue mDiffuseLightColours[NUM_LIGHTS] =
{
    Ogre::ColourValue(0.6, 0.6, 0.6)
};

Ogre::ColourValue mSpecularLightColours[NUM_LIGHTS] =
{
    Ogre::ColourValue(0.5, 0.5, 0.5)
};

// Which lights are enabled
bool mLightState[NUM_LIGHTS] =
{
    true
};

// the light nodes
Ogre::SceneNode* mLightNodes[NUM_LIGHTS];
// the light node pivots
Ogre::SceneNode* mLightPivots[NUM_LIGHTS];



Water::Water()
{
    taskRender = new Task(this, Water::doTaskRender_Wrapper);
    taskRender->_this = this;
    taskRender->_functionPointer = Water::doTaskRender_Wrapper;
}

Water::~Water()
{
    destroy();
}

void Water::initialize(const char *_sSceneManager, const char *_sCamera)
{
    mRoot = NULL;
    mSceneMgr = GraphicManager::getSingleton().getSceneManager(_sSceneManager);
    mCamera = GraphicManager::getSingleton().getCamera(_sSceneManager, _sCamera);

    createScene();
    configureShader();
}

void Water::destroy()
{
    // get rid of the shared pointers before shutting down ogre or exceptions occure
    mActiveFragmentProgram.setNull();
    mActiveFragmentParameters.setNull();
    mActiveVertexProgram.setNull();
    mActiveVertexParameters.setNull();
    mActiveMaterial.setNull();
}

void* Water::doTaskRender(void *pArgs)
{
    float fElapsedTime = *(float *)pArgs;
    (void) fElapsedTime;

    return NULL;
}

void Water::createScene()
{
    // Set ambient light
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.3, 0.3, 0.3));
    mSceneMgr->setSkyBox(true, "Sky/Evening", 100);

    mMainNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();


    for (unsigned int i = 0; i < NUM_LIGHTS; ++i)
    {
        mLightPivots[i] = mSceneMgr->getRootSceneNode()->createChildSceneNode();
        mLightPivots[i]->rotate(mLightRotationAxes[i], Ogre::Angle(mLightRotationAngles[i]));
        // Create a light, use default parameters
        mLights[i] = mSceneMgr->createLight("Light" + Ogre::StringConverter::toString(i));
        mLights[i]->setPosition(mLightPositions[i]);
        mLights[i]->setDiffuseColour(mDiffuseLightColours[i]);
        mLights[i]->setSpecularColour(mSpecularLightColours[i]);
        mLights[i]->setVisible(mLightState[i]);
        //mLights[i]->setAttenuation(400, 0.1 , 1 , 0);
        // Attach light
        mLightPivots[i]->attachObject(mLights[i]);
        // Create billboard for light
        mLightFlareSets[i] = mSceneMgr->createBillboardSet("Flare" + Ogre::StringConverter::toString(i));
        mLightFlareSets[i]->setMaterialName("LightFlare");
        mLightPivots[i]->attachObject(mLightFlareSets[i]);
        mLightFlares[i] = mLightFlareSets[i]->createBillboard(mLightPositions[i]);
        mLightFlares[i]->setColour(mDiffuseLightColours[i]);
        mLightFlareSets[i]->setVisible(mLightState[i]);
    }

    // Define a plane mesh that will be used for the ocean surface
    Ogre::Plane oceanSurface;
    oceanSurface.normal = Ogre::Vector3::UNIT_Y;
    oceanSurface.d = 20;
    Ogre::MeshManager::getSingleton().createPlane("OceanSurface",
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        oceanSurface,
        100000, 100000, 50, 50, true, 1, 10, 10, Ogre::Vector3::UNIT_Z);

    mOceanSurfaceEnt = mSceneMgr->createEntity( "OceanSurface", "OceanSurface" );
    mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(mOceanSurfaceEnt);

}

void Water::configureShader()
{
    mActiveMaterial = Ogre::MaterialManager::getSingleton().getByName("Ocean2_HLSL_GLSL");

    if(!mActiveMaterial.isNull() && mActiveMaterial->getNumSupportedTechniques())
    {
        Ogre::Technique* currentTechnique = mActiveMaterial->getSupportedTechnique(0);
        if(currentTechnique)
        {
            mActivePass = currentTechnique->getPass(0);
            if(mActivePass)
            {
                if (mActivePass->hasFragmentProgram())
                {
                    mActiveFragmentProgram = mActivePass->getFragmentProgram();
                    mActiveFragmentParameters = mActivePass->getFragmentProgramParameters();
                }
                if (mActivePass->hasVertexProgram())
                {
                    mActiveVertexProgram = mActivePass->getVertexProgram();
                    mActiveVertexParameters = mActivePass->getVertexProgramParameters();
                }
            }
        }
    }

    if (mOceanSurfaceEnt) {
        mOceanSurfaceEnt->setMaterialName("Ocean2_HLSL_GLSL");
    }
}

#endif  // S_ENABLE_GRAPHIC
