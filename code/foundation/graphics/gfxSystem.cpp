#include "gfxSystem.h"

// S_ENABLE_GRAPHIC is defined in premake.lua, it should only be altered there
// to allow for easy stripping/adding of libraries actually needed to be compiled
#if defined(S_ENABLE_GRAPHIC)

using namespace Foundation;
using namespace Foundation::Graphic;

// Static data members
Task                                    *GraphicManager::m_pTask;
Ogre::Root                              *GraphicManager::m_pRoot;
RenderWindow                            *GraphicManager::m_pWindow;
Ogre::String                            GraphicManager::m_sResourcePath;
Ogre::RaySceneQuery                     *GraphicManager::m_pRaySceneQuery;
SelectionRectangle                      *GraphicManager::m_pRectSelection;
Ogre::PlaneBoundedVolumeListSceneQuery  *GraphicManager::m_pVolQuery;

vector<Ogre::SceneManager *> GraphicManager::m_uSceneManagerList;
vector<Ogre::Camera *>       GraphicManager::m_uCameraList;
vector<SelectionObject *>    GraphicManager::m_uSelectedObjectList;

// SelectionObject
char* SelectionObject::getName()
{
    return (char*)MovableObject::getName().c_str();
}

// Camera

// GraphicManager
GraphicManager& GraphicManager::getSingleton()
{
    static GraphicManager m_uSingleton;

    return m_uSingleton;
}

GraphicManager::GraphicManager()
{
    m_sResourcePath = "";
    m_pRoot = NULL;
    m_pVolQuery = NULL;
    m_pRectSelection = NULL;

    m_pTask = new Task(this, GraphicManager::doTaskRender_Wrapper);
    //m_pTask->_this = this;
    //m_pTask->_functionPointer = GraphicManager::doTaskRender_Wrapper;

    Channel_Join("GRAPHICS_SELECTION", GraphicManager::doChannelReceive_Wrapper);
}

GraphicManager::~GraphicManager()
{
    destroy();
}

void* GraphicManager::doTaskRender(void *_pArgs)
{
    float nElapsedTime = *(float *)_pArgs;
    (void) nElapsedTime;

    Ogre::WindowEventUtilities::messagePump();
    m_pRoot->renderOneFrame();

    return NULL;
}

bool GraphicManager::initialize(const char *_sWindowTitle)
{
    Ogre::String sPluginPath;

#if FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_WIN
    sPluginPath = m_sResourcePath + "plugins_win.cfg";
#elif FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_LINUX
    sPluginPath = m_sResourcePath + "plugins_linux.cfg";
#else
    #error Plugin path not defined for this OS.
#endif

    // Suppress Ogre output to a file
    f_printf(" - [GraphicManager] Creating Ogre Log...\n");
    Ogre::LogManager *pOgreLog = new Ogre::LogManager();
    (void) pOgreLog;
    Ogre::LogManager::getSingleton().createLog("OgreOut.log", false, false, false);

    // Create the root node
    m_pRoot = new Ogre::Root(sPluginPath, m_sResourcePath + "ogre.cfg", m_sResourcePath + "Ogre.log");

    // Setup resources
    f_printf(" - [GraphicManager] Setting up Ogre resources...\n");
    setupResources();

    // Configure our device
    f_printf(" - [GraphicManager] Configuring Ogre...\n");
    if (!configure(_sWindowTitle)) {
        f_printf(" - [GraphicManager]  Configure returned an error, aborting.\n");
        return false;
    }

    // Load resources
    f_printf(" - [GraphicManager] Loading Ogre resources...\n");
    loadResources();

    // Create the scene
    createScene();

    return true;
}

void GraphicManager::destroy()
{
    GraphicManager::getSingleton().Channel_Leave("GRAPHICS_SELECTION", GraphicManager::doChannelReceive_Wrapper);

    if (m_pRoot) {
        if (m_pVolQuery)
            (*m_uSceneManagerList.begin())->destroyQuery(m_pVolQuery);
        if (m_pRectSelection)
            delete m_pRectSelection;
        m_pRoot->shutdown();
        delete m_pRoot;
        m_pRoot = NULL;
    }
}

bool GraphicManager::configure(const char *_sWindowTitle)
{
    // Create a new window
    bool bRestored = true;

    /*
    Ogre::RenderSystem *rs = m_pRoot->getRenderSystemByName("OpenGL Rendering Subsystem");
    rs->setConfigOption("Full Screen", "Yes");
    rs->setConfigOption("Video Mode", "1280 x 1024 @ 32-bit colour");
    rs->setConfigOption("RTT Preferred Mode", "PBuffer");
    m_pRoot->setRenderSystem(rs);
    */

    bRestored = m_pRoot->restoreConfig();

    if (!bRestored) {
        f_printf("[GraphicManager::configure] Error: Could not restore previous OGRE configuration.\n");
        return false;
    }

    m_pWindow = (RenderWindow *)m_pRoot->initialise(true, _sWindowTitle);

    if (!m_pWindow) {
        f_printf("Error creating window.\n");
        return false;
    }

    return true;
}

void GraphicManager::setupResources()
{
    // Load resource paths from config file
    Ogre::ConfigFile cf;
    cf.load(m_sResourcePath + "resources.cfg");

    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

    Ogre::String secName, typeName, archName;
    while (seci.hasMoreElements()) {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i) {
            typeName = i->first;
            archName = i->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
        }
    }
}

void GraphicManager::loadResources()
{
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

void GraphicManager::showCursor(const bool _bShowCursor)
{
    #if FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_WIN
    ShowCursor(_bShowCursor);
    #endif
}

void GraphicManager::createLight(const char *_sSceneManagerName, const char *_sLightName, const gmtl::Vec3f _nPosition, const gmtl::Vec3f _nDiffuse, const gmtl::Vec3f _nSpecular)
{
    Ogre::SceneManager  *pSceneManager;
    Ogre::Light         *pLight;

    if (m_pRoot) {
        pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);
        if (pSceneManager) {
            pLight = pSceneManager->createLight(_sLightName);
            pLight->setPosition(Ogre::Vector3(_nPosition[gmtl::Xelt], _nPosition[gmtl::Yelt], _nPosition[gmtl::Zelt]));
            pLight->setDiffuseColour(_nDiffuse[0], _nDiffuse[1], _nDiffuse[2]);
            pLight->setSpecularColour(_nSpecular[0], _nSpecular[1], _nSpecular[2]);
            pLight->setVisible(true);
            pLight->setCastShadows(true);
        }
    }
}

void GraphicManager::clearBloomCompositors()
{
    for (int i = 0; i < m_pWindow->getNumViewports(); i++) {
        Ogre::Viewport *pViewport = m_pWindow->getViewport(i);
    
        Ogre::CompositorManager::getSingleton().removeCompositor(pViewport, "Bloom");
    }
}

void GraphicManager::enableBloom(const bool _bEnabled)
{
    for (int i = 0; i < m_pWindow->getNumViewports(); i++) {
        Ogre::Viewport *pViewport = m_pWindow->getViewport(i);
    
        Ogre::CompositorManager::getSingleton().addCompositor(pViewport, "Bloom");
        Ogre::CompositorManager::getSingleton().setCompositorEnabled(pViewport, "Bloom", _bEnabled);
    }
}

void GraphicManager::setShadowTechnique(const char *_sSceneManagerName)
{
    Ogre::SceneManager  *pSceneManager;

    if (m_pRoot) {
        pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);
        if (pSceneManager) {
            pSceneManager->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE);
            pSceneManager->setShadowTextureSelfShadow(true);
        }
    }
}

void GraphicManager::createScene()
{
}

// Particle System
void GraphicManager::addParticleSystem(const char *_sSceneManagerName, const char *_sParticleSystemName, const char *_sAttachEntityName, const char *_sScriptName)
{
	Ogre::SceneManager *pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);
	Ogre::ParticleSystem *pParticleSystem;
	Ogre::SceneNode	*pNode, *pNodeAttach;
    Ogre::Entity *pEntity;

	if (pSceneManager) {
        if (pSceneManager->hasEntity(_sAttachEntityName)) {
            pEntity = pSceneManager->getEntity(_sAttachEntityName);
            pNodeAttach = pEntity->getParentSceneNode();
            
            if (pSceneManager->hasParticleSystem(_sParticleSystemName))
                pSceneManager->destroyParticleSystem(_sParticleSystemName);

	        pParticleSystem = pSceneManager->createParticleSystem(_sParticleSystemName, _sScriptName);
            pNode = pNodeAttach->createChildSceneNode();
            pParticleSystem->setVisible(true);
            pNode->attachObject(pParticleSystem);
        } else {
    		f_printf("[GraphicManager::addParticleSystem] Error: Entity %s doesn't exist.\n", _sAttachEntityName);
        }
	} else {
		f_printf("[GraphicManager::addParticleSystem] Error: SceneManager %s doesn't exist.\n", _sSceneManagerName);
	}
}

void GraphicManager::destroyParticleSystem(const char *_sSceneManagerName, const char *_sParticleSystemName)
{
	Ogre::SceneManager *pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);

	if (pSceneManager) {
        if (pSceneManager->hasParticleSystem(_sParticleSystemName))
            pSceneManager->destroyParticleSystem(_sParticleSystemName);
        else
    		f_printf("[GraphicManager::addParticleSystem] Error: ParticleSystem %s doesn't exist.\n", _sParticleSystemName);            
	} else {
		f_printf("[GraphicManager::addParticleSystem] Error: SceneManager %s doesn't exist.\n", _sSceneManagerName);
	}
}

void GraphicManager::showParticleSystem(const char *_sSceneManagerName, const char *_sParticleSystemName, const bool _bVisible)
{
    Ogre::SceneManager *pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);
	Ogre::ParticleSystem *pParticleSystem;

	if (pSceneManager) {
		pParticleSystem = pSceneManager->getParticleSystem(_sParticleSystemName);
		if (pParticleSystem) {
            pParticleSystem->setVisible(_bVisible);
        }
    }
}

void GraphicManager::enableParticleSystemEmitter(const char *_sSceneManagerName, const char *_sParticleSystemName, const unsigned int _nEmitterIndex, const bool _bVisible)
{
    Ogre::SceneManager *pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);
	Ogre::ParticleSystem *pParticleSystem;
    Ogre::ParticleEmitter *pEmitter;

	if (pSceneManager) {
		pParticleSystem = pSceneManager->getParticleSystem(_sParticleSystemName);
		if (pParticleSystem) {
            pEmitter = pParticleSystem->getEmitter(_nEmitterIndex);
            if (pEmitter) {
                pEmitter->setEnabled(_bVisible);
            }
        }
    }
}

void GraphicManager::translateParticleSystem(const char *_sSceneManagerName, const char *_sParticleSystemName, gmtl::Vec3f _nPosition)
{
	Ogre::SceneManager *pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);
	Ogre::ParticleSystem *pParticleSystem;
	Ogre::SceneNode	*pNode;

	if (pSceneManager) {
        if (pSceneManager->hasParticleSystem(_sParticleSystemName)) {
		    pParticleSystem = pSceneManager->getParticleSystem(_sParticleSystemName);
			pNode = pParticleSystem->getParentSceneNode();
            if (pNode) {
    			pNode->translate(_nPosition[gmtl::Xelt], _nPosition[gmtl::Yelt], _nPosition[gmtl::Zelt]);
            } else {
                f_printf("[GraphicManager::translateParticleSystem] Error: ParticleSystem parent doesn't exist.\n");
            }
		} else {
			f_printf("[GraphicManager::translateParticleSystem] Error: ParticleSystem %s doesn't exist.\n", _sParticleSystemName);
		}
	} else {
		f_printf("[GraphicManager::translateParticleSystem] Error: SceneManager %s doesn't exist.\n", _sSceneManagerName);
	}
}

void GraphicManager::rotateParticleSystem(const char *_sSceneManagerName, const char *_sParticleSystemName, gmtl::Quatf _nRotation)
{
	Ogre::SceneManager *pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);
	Ogre::ParticleSystem *pParticleSystem;
	Ogre::SceneNode	*pNode;

	if (pSceneManager) {
		pParticleSystem = pSceneManager->getParticleSystem(_sParticleSystemName);
		if (pParticleSystem) {
			pNode = pParticleSystem->getParentSceneNode();
			pNode->rotate(Ogre::Quaternion(_nRotation[gmtl::Xelt], _nRotation[gmtl::Yelt], _nRotation[gmtl::Zelt], _nRotation[gmtl::Welt]));
		} else {
			f_printf("[GraphicManager::addParticleSystem] Error: ParticleSystem %s doesn't exist.\n", _sParticleSystemName);
		}
	} else {
		f_printf("[GraphicManager::addParticleSystem] Error: SceneManager %s doesn't exist.\n", _sSceneManagerName);
	}
}

void GraphicManager::fastForwardParticleSystem(const char *_sSceneManagerName, const char *_sParticleSystemName, float _nTime)
{
	Ogre::SceneManager *pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);
	Ogre::ParticleSystem *pParticleSystem;

	if (pSceneManager) {
		pParticleSystem = pSceneManager->getParticleSystem(_sParticleSystemName);
		if (pParticleSystem) {
			pParticleSystem->fastForward(_nTime);
		} else {
			f_printf("[GraphicManager::addParticleSystem] Error: ParticleSystem %s doesn't exist.", _sParticleSystemName);
		}
	} else {
		f_printf("[GraphicManager::addParticleSystem] Error: SceneManager %s doesn't exist.", _sSceneManagerName);
	}
}

// Camera
Camera* GraphicManager::addCamera(const char *_sSceneManagerName, const char *_sCameraName, const int _nZOrder, const float _nViewportLeft, const float _nViewportTop, const float _nViewportWidth, const float _nViewportHeight)
{
    Ogre::SceneManager *pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);
    Ogre::Camera *pCamera = pSceneManager->createCamera(_sCameraName);

    m_pRoot->getAutoCreatedWindow()->removeViewport(_nZOrder);
    m_pRoot->getAutoCreatedWindow()->addViewport(pCamera, _nZOrder, _nViewportLeft, _nViewportTop, _nViewportWidth, _nViewportHeight);

    pCamera->setFarClipDistance(10000.0f);
    pCamera->setNearClipDistance(0.01f);

    // Add the camera to our list
    m_uCameraList.push_back(pCamera);

    return (Camera *)pCamera;
}

void GraphicManager::removeCamera(const char *_sSceneManagerName, const char *_sCameraName)
{
    Ogre::SceneManager *pSceneManager;
    Ogre::Camera *pCamera;
    vector<Ogre::Camera *>::iterator itrVec;
    Ogre::Viewport *pViewport;

    if (m_pRoot) {
        pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);
        if (pSceneManager) {
            if (pSceneManager->hasCamera(_sCameraName)) {
                pCamera = pSceneManager->getCamera(_sCameraName);
                pViewport = pCamera->getViewport();
                m_pRoot->getAutoCreatedWindow()->removeViewport(pViewport->getZOrder());
                pSceneManager->destroyCamera(pCamera);
            }
        }
    }

    // Remove the camera from our list
    for (itrVec = m_uCameraList.begin(); itrVec != m_uCameraList.end(); itrVec++) {
        if ((*itrVec) == pCamera) {
            m_uCameraList.erase(itrVec);
            break;
        }
    }
}

void GraphicManager::setCameraPosition(const char *_sSceneManagerName, const char *_sCameraName, gmtl::Vec3f _nPosition)
{
    Ogre::SceneManager *pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);
    Ogre::Camera *pCamera;
    Ogre::Vector3 nPosition = Ogre::Vector3(_nPosition[gmtl::Xelt], _nPosition[gmtl::Yelt], _nPosition[gmtl::Zelt]);

    if (pSceneManager->hasCamera(_sCameraName)) {
        pCamera = pSceneManager->getCamera(_sCameraName);
        pCamera->setPosition(nPosition);
    }
}

gmtl::Vec3f GraphicManager::getCameraPosition(const char *_sSceneManagerName, const char *_sCameraName) const
{
    Ogre::SceneManager *pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);
    Ogre::Camera *pCamera = pSceneManager->getCamera(_sCameraName);
    Ogre::Vector3 nPosition = pCamera->getPosition();

    return gmtl::Vec3f(nPosition[0], nPosition[1], nPosition[2]);
}

void GraphicManager::setCameraRotation(const char *_sSceneManagerName, const char *_sCameraName, gmtl::Vec3f _nAxis, float _nDegreeAngle)
{
    Ogre::SceneManager *pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);
    Ogre::Camera *pCamera = pSceneManager->getCamera(_sCameraName);
    Ogre::Quaternion nRotation = Ogre::Quaternion(1,0,0,0); //Ogre::Quaternion(Ogre::Degree(_nDegreeAngle), Ogre::Vector3(_nAxis[gmtl::Xelt], _nAxis[gmtl::Yelt], _nAxis[gmtl::Zelt]));

    pCamera->setOrientation(nRotation);
}

void GraphicManager::setCameraDirection(const char *_sSceneManagerName, const char *_sCameraName, gmtl::Vec3f _nDirection)
{
    Ogre::SceneManager *pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);
    Ogre::Camera *pCamera = pSceneManager->getCamera(_sCameraName);

    pCamera->setDirection(Ogre::Vector3(_nDirection[gmtl::Xelt], _nDirection[gmtl::Yelt], _nDirection[gmtl::Zelt]));
}

void GraphicManager::setCameraTracking(const char *_sSceneManagerName, const char *_sCameraName, const bool _bEnabled, const char *_sObjectTrack = "", const gmtl::Vec3f _nOffset = gmtl::Vec3f(0,0,0))
{
    Ogre::SceneManager *pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);
    //Ogre::Camera *pCamera = pSceneManager->getCamera(_sCameraName);
    Ogre::SceneNode *pNodeAttach;
    Ogre::Entity *pEntity;

    if (pSceneManager) {
        if (pSceneManager->hasEntity(_sObjectTrack)) {
            pEntity = pSceneManager->getEntity(_sObjectTrack);
            pNodeAttach = pEntity->getParentSceneNode();
            //pCamera->detatchFromParent();
            //pNodeAttach->attachObject(pCamera);
            //pCamera->setAutoTracking(_bEnabled, pNodeAttach, Ogre::Vector3(_nOffset[gmtl::Xelt], _nOffset[gmtl::Yelt], _nOffset[gmtl::Zelt]));
        }
    }
}

void GraphicManager::setCameraLookat(const char *_sSceneManagerName, const char *_sCameraName, gmtl::Vec3f _nPosition)
{
    Ogre::SceneManager *pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);
    Ogre::Camera *pCamera;

    if (pSceneManager->hasCamera(_sCameraName)) {
        pCamera = pSceneManager->getCamera(_sCameraName);
        pCamera->lookAt(_nPosition[gmtl::Xelt], _nPosition[gmtl::Yelt], _nPosition[gmtl::Zelt]);
    }
}

gmtl::Quatf GraphicManager::getCameraRotation(const char *_sSceneManagerName, const char *_sCameraName) const
{
    Ogre::SceneManager *pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);
    Ogre::Camera *pCamera = pSceneManager->getCamera(_sCameraName);
    Ogre::Quaternion nRotation = pCamera->getOrientation();

    return gmtl::Quatf(nRotation.x, nRotation.y, nRotation.z, nRotation.w);
}

Camera* GraphicManager::getCamera(const char *_sSceneManagerName, const char *_sCameraName) const
{
    Ogre::SceneManager *pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);
    Camera *pCamera;

    if (pSceneManager->hasCamera(_sCameraName))
        pCamera = (Camera *)pSceneManager->getCamera(_sCameraName);
    else
        return NULL;

    return pCamera;
}

void GraphicManager::addMesh(const char *_sSceneManagerName, const char *_sID, const char *_sMeshFilename, unsigned int _nQueryFlags)
{
    Ogre::SceneManager  *pSceneManager = NULL;
    Ogre::Entity        *pEntity = NULL;
    Ogre::SceneNode     *pNode = NULL;

    if (m_pRoot) {
        pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);
    
        if (pSceneManager) {
            if (pSceneManager->hasEntity(_sID)) {
                pEntity = pSceneManager->getEntity(_sID);
                pNode = pEntity->getParentSceneNode();
                pNode->detachObject(pEntity);
            } else {
                pEntity = pSceneManager->createEntity(_sID, _sMeshFilename);
            }
            //pEntity->setCastShadows(true);
            pEntity->setQueryFlags(_nQueryFlags);
            pNode = pSceneManager->getRootSceneNode()->createChildSceneNode();
            pNode->attachObject(pEntity);
            pNode->showBoundingBox(false);
        } else {
            f_printf("[GraphicManager] Error: Could not find SceneManager: %s\n", _sSceneManagerName);
        }
    } else {
        f_printf("[GraphicManager] Error: Adding mesh when not initialized.\n");
    }
}

void GraphicManager::removeMesh(const char *_sSceneManagerName, const char *_sID)
{
    Ogre::SceneManager  *pSceneManager = NULL;
    Ogre::Entity        *pEntity = NULL;
    Ogre::SceneNode     *pNode = NULL;

    if (m_pRoot) {
        pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);
    
        if (pSceneManager) {
            if (pSceneManager->hasEntity(_sID)) {
                pEntity = pSceneManager->getEntity(_sID);
                pNode = pEntity->getParentSceneNode();
                pSceneManager->destroyEntity(pEntity);
                pNode->removeAndDestroyAllChildren();
            }
        } else {
            f_printf("[GraphicManager] Error: Could not find SceneManager: %s\n", _sSceneManagerName);
        }
    } else {
        f_printf("[GraphicManager] Error: Adding mesh when not initialized.\n");
    }
}

void GraphicManager::setMeshPosition(const char *_sSceneManagerName, const char *_sID, gmtl::Vec3f _nPosition)
{
    Ogre::SceneManager *pSceneManager = NULL;
    Ogre::Entity *pEntity = NULL;
    Ogre::SceneNode *pNode = NULL;

    if (m_pRoot) {
        pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);
        if (pSceneManager) {
            pEntity = pSceneManager->getEntity(_sID);
            pNode = pEntity->getParentSceneNode();
            pNode->setPosition(Ogre::Vector3(_nPosition[0], _nPosition[1], _nPosition[2]) - pEntity->getBoundingBox().getCenter());
        } else {
            f_printf("[GraphicManager] Error: Could not find SceneManager: %s\n", _sSceneManagerName);
        }
    } else {
        f_printf("[GraphicManager] Error: Setting mesh position when not initialized.\n");
    }
}

gmtl::Quatf GraphicManager::getMeshRotation(const char *_sSceneManagerName, const char *_sID)
{
    Ogre::SceneManager *pSceneManager = NULL;
    Ogre::Entity *pEntity = NULL;
    Ogre::SceneNode *pNode = NULL;
    gmtl::Quatf nReturn(0,0,0,1);

    if (m_pRoot) {
        pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);
        if (pSceneManager) {
            pEntity = pSceneManager->getEntity(_sID);
            pNode = pEntity->getParentSceneNode();

            Ogre::Quaternion r = pNode->getOrientation();
            nReturn = gmtl::Quatf(r.x, r.y, r.z, r.w);
        } else {
            f_printf("[GraphicManager] Error: Could not find SceneManager: %s\n", _sSceneManagerName);
        }
    } else {
        f_printf("[GraphicManager] Error: Setting mesh position when not initialized.\n");
    }

    return nReturn;
}

void GraphicManager::setMeshRotation(const char *_sSceneManagerName, const char *_sID, gmtl::Quatf _nRotation)
{
    Ogre::SceneManager *pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);
    Ogre::Entity *pEntity = pSceneManager->getEntity(_sID);
    Ogre::SceneNode *pNode = pEntity->getParentSceneNode();

    if (m_pRoot) {
        pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);
        if (pSceneManager) {
            pEntity = pSceneManager->getEntity(_sID);
            pNode = pEntity->getParentSceneNode();

            Ogre::Quaternion r(_nRotation[gmtl::Welt], _nRotation[gmtl::Xelt], _nRotation[gmtl::Yelt], _nRotation[gmtl::Zelt]);

            //q = r * q;
            //pNode->setOrientation(q);
            pNode->setOrientation(r);
        } else {
            f_printf("[GraphicManager] Error: Could not find SceneManager: %s\n", _sSceneManagerName);
        }
    } else {
        f_printf("[GraphicManager] Error: Setting mesh rotation when not initialized.\n");
    }
}

void GraphicManager::setMeshRotation(const char *_sSceneManagerName, const char *_sID, const gmtl::Vec3f _nAxis, const float _nDegreeAngle)
{
    Ogre::SceneManager *pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);
    Ogre::Entity *pEntity = pSceneManager->getEntity(_sID);
    Ogre::SceneNode *pNode = pEntity->getParentSceneNode();

    if (m_pRoot) {
        pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);
        if (pSceneManager) {
            pEntity = pSceneManager->getEntity(_sID);
            pNode = pEntity->getParentSceneNode();

            pNode->rotate(Ogre::Vector3(_nAxis[gmtl::Xelt], _nAxis[gmtl::Yelt], _nAxis[gmtl::Zelt]), Ogre::Degree(_nDegreeAngle));
        } else {
            f_printf("[GraphicManager] Error: Could not find SceneManager: %s\n", _sSceneManagerName);
        }
    } else {
        f_printf("[GraphicManager] Error: Setting mesh rotation when not initialized.\n");
    }
}

void GraphicManager::setMeshScale(const char *_sSceneManagerName, const char *_sID, gmtl::Vec3f _nScale)
{
    Ogre::SceneManager *pSceneManager = NULL;
    Ogre::Entity *pEntity = NULL;
    Ogre::SceneNode *pNode = NULL;
    
    if (m_pRoot) {
        pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);
        if (pSceneManager) {
            pEntity = pSceneManager->getEntity(_sID);
            pNode = pEntity->getParentSceneNode();

            pNode->setScale(_nScale[gmtl::Xelt], _nScale[gmtl::Yelt], _nScale[gmtl::Zelt]);
        } else {
            f_printf("[GraphicManager] Error: Could not find SceneManager: %s\n", _sSceneManagerName);
        }
    } else {
        f_printf("[GraphicManager] Error: Setting mesh scale when not initialized.\n");
    }
}

void GraphicManager::setMeshMaterial(const char *_sSceneManagerName, const char *_sID, const char *_sMaterialName)
{
    Ogre::SceneManager *pSceneManager = NULL;
    Ogre::Entity *pEntity = NULL;

    if (m_pRoot) {
        pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);
        if (pSceneManager) {
            pEntity = pSceneManager->getEntity(_sID);
            pEntity->setMaterialName(_sMaterialName);
        } else {
            f_printf("[GraphicManager] Error: Could not find SceneManager: %s\n", _sSceneManagerName);
        }
    } else {
        f_printf("[GraphicManager] Error: Setting mesh material when not initialized.\n");
    }
}

void GraphicManager::setMeshVisibility(const char *_sSceneManagerName, const char *_sID, const bool _bVisible)
{
    Ogre::SceneManager *pSceneManager = NULL;
    Ogre::Entity *pEntity = NULL;

    if (m_pRoot) {
        pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);
        if (pSceneManager) {
            pEntity = pSceneManager->getEntity(_sID);

            pEntity->setVisible(_bVisible);
        } else {
            f_printf("[GraphicManager] Error: Could not find SceneManager: %s\n", _sSceneManagerName);
        }
    } else {
        f_printf("[GraphicManager] Error: Setting mesh material when not initialized.\n");
    }
}

void GraphicManager::destroyAllEntities(const char *_sSceneManagerName)
{
    Ogre::SceneManager *pSceneManager = NULL;

    if (m_pRoot) {
        pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);
        if (pSceneManager) {
            pSceneManager->destroyAllEntities();
            pSceneManager->destroyAllManualObjects();
        } else {
            f_printf("[GraphicManager] Error: Could not find SceneManager: %s\n", _sSceneManagerName);
        }
    } else {
        f_printf("[GraphicManager] Error: Setting mesh material when not initialized.\n");
    }
}

void GraphicManager::setSkybox(const char *_sSceneManagerName, const char *_sSkyboxName)
{
    Ogre::SceneManager *pSceneManager = NULL;

    if (m_pRoot) {
        pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);
        if (pSceneManager) {
            pSceneManager->setSkyBox(true, _sSkyboxName);
        } else {
            f_printf("[GraphicManager] Error: Could not find SceneManager: %s\n", _sSceneManagerName);
        }
    } else {
        f_printf("[GraphicManager] Error: Setting mesh material when not initialized.\n");
    }
}

void GraphicManager::addLine(const char *_sSceneManagerName, const char *_sID, vector<gmtl::Vec3f> _nPoints, float _nRed, float _nGreen, float _nBlue)
{
    if (!m_pRoot) {
        f_printf("[GraphicManager] Error: Adding line when not initialized.\n");
        return;
    }

    Ogre::SceneManager *pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);
    Ogre::ManualObject *myManualObject;
    Ogre::MaterialPtr myManualObjectMaterial;
    Ogre::SceneNode *myManualObjectNode;
    vector<gmtl::Vec3f>::iterator itrVec;
    
    // Create the OGRE names based on the ID
    Ogre::String sManualObjectName = Ogre::String("ManualObjectCollisionLine") + _sID;
    Ogre::String sManualObjectMaterial = Ogre::String("ManualObjectMaterial") + _sID;

    // Get/Create the manualobject containing the lines
    if (pSceneManager->hasManualObject(sManualObjectName))
        myManualObject = pSceneManager->getManualObject(sManualObjectName);
    else
        myManualObject = pSceneManager->createManualObject(sManualObjectName);

    myManualObject->setQueryFlags(0);

    // Get/Create the scenenode holding the manualobject
    if (pSceneManager->hasSceneNode(_sID)) {
        myManualObjectNode = pSceneManager->getSceneNode(_sID);
    } else {
        myManualObjectNode = pSceneManager->getRootSceneNode()->createChildSceneNode(_sID);
        myManualObjectNode->attachObject(myManualObject);
    }

    // Get/Create the manualobject's material
    myManualObjectMaterial = Ogre::MaterialManager::getSingleton().getByName(sManualObjectMaterial);
    if (myManualObjectMaterial.isNull())
        myManualObjectMaterial = Ogre::MaterialManager::getSingleton().create(sManualObjectMaterial, "Debug_Collision");

    // Setup material properties
    myManualObjectMaterial->setReceiveShadows(false);
    myManualObjectMaterial->getTechnique(0)->setLightingEnabled(true);
    myManualObjectMaterial->getTechnique(0)->getPass(0)->setDiffuse(0,0,0,1);
    myManualObjectMaterial->getTechnique(0)->getPass(0)->setAmbient(_nRed, _nGreen, _nBlue);
    myManualObjectMaterial->getTechnique(0)->getPass(0)->setSelfIllumination(_nRed, _nGreen, _nBlue);
    
    // Add the line points to the manualobject
    myManualObject->clear();
    myManualObject->begin(sManualObjectMaterial, Ogre::RenderOperation::OT_LINE_STRIP);
    for (itrVec = _nPoints.begin(); itrVec != _nPoints.end(); itrVec++) {
        myManualObject->position((*itrVec)[gmtl::Xelt], (*itrVec)[gmtl::Yelt], (*itrVec)[gmtl::Zelt]);
    }
    myManualObject->end();
}

void GraphicManager::clearLine(const char *_sSceneManagerName, const char *_sID)
{
    Ogre::SceneManager *pSceneManager;
    Ogre::ManualObject *myManualObject;
    Ogre::String sManualObjectName = Ogre::String("ManualObjectCollisionLine") + _sID;
    Ogre::String sManualObjectMaterial = Ogre::String("ManualObjectMaterial") + _sID;

    if (m_pRoot) {
        pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);
        if (pSceneManager) {
            if (pSceneManager->hasManualObject(sManualObjectName)) {
                myManualObject = pSceneManager->getManualObject(sManualObjectName);
                if (myManualObject) {
                    myManualObject->clear();
                    myManualObject->begin(sManualObjectMaterial, Ogre::RenderOperation::OT_LINE_STRIP);
                    myManualObject->end();
                }
            } else {
                f_printf("[Foundation.GraphicManager] Error: ManualObject is null.\n");
            }
        }
    }
}

void GraphicManager::destroyLine(const char *_sSceneManagerName, const char *_sID)
{
    Ogre::SceneManager *pSceneManager;
    Ogre::ManualObject *myManualObject;
    Ogre::String sManualObjectName = Ogre::String("ManualObjectCollisionLine") + _sID;

    if (m_pRoot) {
        pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);
        if (pSceneManager) {
            if (pSceneManager->hasManualObject(sManualObjectName)) {
                myManualObject = pSceneManager->getManualObject(sManualObjectName);
                if (myManualObject) {
                    //myManualObject->getParentSceneNode()->detachObject(myManualObject);
                    pSceneManager->destroyManualObject(myManualObject);
                } else {
                    f_printf("[Foundation.GraphicManager] Error: ManualObject is null.\n");
                }
            } else {
                //f_printf("[Foundation.GraphicManager] Error: SceneManager doesn't have ManualObject %s.\n", sManualObjectName.c_str());
            }
        } else {
            f_printf("[Foundation.GraphicManager] Error: SceneManager is null.\n");
        }
    } else {
        f_printf("[Foundation.GraphicManager] Error: Root is null.\n");
    }
}

void GraphicManager::updateLine(const char *_sSceneManagerName, const char *_sID, vector<gmtl::Vec3f> _nPoints)
{
    Ogre::SceneManager *pSceneManager;
    Ogre::ManualObject *myManualObject;
    Ogre::String sManualObjectName = Ogre::String("ManualObjectCollisionLine") + _sID;
    vector<gmtl::Vec3f>::iterator itrVec;

    if (m_pRoot) {
        pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);
        if (pSceneManager) {
            if (pSceneManager->hasManualObject(sManualObjectName)) {
                myManualObject = pSceneManager->getManualObject(sManualObjectName);

                if (myManualObject) {
                    myManualObject->beginUpdate(0);
                    for (itrVec = _nPoints.begin(); itrVec != _nPoints.end(); itrVec++) {
                        myManualObject->position((*itrVec)[gmtl::Xelt], (*itrVec)[gmtl::Yelt], (*itrVec)[gmtl::Zelt]);
                    }
                    myManualObject->end();
                }
            } else {
                f_printf("[GraphicManager::updateLine] Error: ManualObject %s does not exist.\n", sManualObjectName.c_str());
            }
        } else {
            f_printf("[GraphicManager::updateLine] Error: Scenemanager %s does not exist.\n", _sSceneManagerName);
        }
    } else {
        f_printf("[GraphicManager::updateLine] Error: OGRE Root is null.\n");
    }
}

void GraphicManager::addPlane(const char *_sSceneManagerName, const char *_sID, float _nWidth, float _nHeight, unsigned int _nSegmentsX, unsigned int _nSegmentsY)
{
    Ogre::String sEntID = Ogre::String(_sID);
    Ogre::Plane plane;
    Ogre::SceneManager *pSceneManager;

    plane.normal = Ogre::Vector3::UNIT_Z;
    plane.d = 20;

    if (m_pRoot) {
        pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);
        if (pSceneManager) {
            if (!pSceneManager->hasEntity(_sID)) {
                Ogre::MeshPtr planeMesh = Ogre::MeshManager::getSingleton().createPlane(_sID, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                    plane, _nWidth, _nHeight, _nSegmentsX, _nSegmentsY, true, 1, 1, 1, Ogre::Vector3::UNIT_Y);
                Ogre::Entity *mPlaneEnt = pSceneManager->createEntity(sEntID, _sID);
                pSceneManager->getRootSceneNode()->createChildSceneNode()->attachObject(mPlaneEnt);
            }
        } else {
            f_printf("[GraphicManager::addPlane] Error: Scenemanager %s does not exist.\n", _sSceneManagerName);
        }
    } else {
        f_printf("[GraphicManager::updateLine] Error: OGRE Root is null.\n");
    }
}

void GraphicManager::addSceneManager(const char *_sSceneManagerName)
{
    Ogre::SceneManager *pSceneManager = m_pRoot->createSceneManager(Ogre::ST_EXTERIOR_CLOSE, _sSceneManagerName);

    // Add the scene manager to our list
    m_uSceneManagerList.push_back(pSceneManager);

    // Create our selection/volumequery for selection box
    if (m_uSceneManagerList.size() > 0) {
        m_pRectSelection = new SelectionRectangle("SelectionRectangle");
    
        // just use the first scenemanager for now
        (*m_uSceneManagerList.begin())->getRootSceneNode()->createChildSceneNode()->attachObject(m_pRectSelection);
    
        m_pVolQuery = (*m_uSceneManagerList.begin())->createPlaneBoundedVolumeQuery(Ogre::PlaneBoundedVolumeList());
    }
}

void GraphicManager::removeSceneManager(const char *_sSceneManagerName)
{
    Ogre::SceneManager *pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);
    vector<Ogre::SceneManager *>::iterator itrVec;

    m_pRoot->destroySceneManager(pSceneManager);

    // Remove the scene manager from our list
    for (itrVec = m_uSceneManagerList.begin(); itrVec != m_uSceneManagerList.end(); itrVec++) {
        if ((*itrVec) == pSceneManager) {
            m_uSceneManagerList.erase(itrVec);
            break;
        }
    }
}

SceneManager* GraphicManager::getSceneManager(const char *_sSceneManagerName) const
{
    Ogre::SceneManager *pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);

    return (SceneManager *)pSceneManager;
}

RenderWindow* GraphicManager::getRenderWindow() const
{
    return m_pWindow;
}

size_t GraphicManager::getWindowHandle() const
{
    size_t windowHnd;
    getRenderWindow()->getCustomAttribute("WINDOW", &windowHnd);
    
    return windowHnd;
}

void GraphicManager::createRibbonTrail(const char *_sSceneManagerName, const char *_sObjectAttachName, const char *_sRibbonTrailName, const char *_sMaterial, const gmtl::Vec3f _nPosition)
{
    Ogre::SceneManager *pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);
    Ogre::RibbonTrail *pRibbonTrail;

    if (pSceneManager) {
        pRibbonTrail = pSceneManager->createRibbonTrail(_sRibbonTrailName);
        pRibbonTrail->setMaterialName(_sMaterial);
        pRibbonTrail->setMaxChainElements(30);
        //pRibbonTrail->setNumberOfChains(110);
        pRibbonTrail->setTrailLength(100);

        Ogre::Entity *pEntity = pSceneManager->getEntity(_sObjectAttachName);
        Ogre::SceneNode *pAttachParent = pEntity->getParentSceneNode();

        pSceneManager->getRootSceneNode()->attachObject(pRibbonTrail);
        Ogre::SceneNode *pTranslatedNode = pAttachParent->createChildSceneNode();

        pTranslatedNode->setPosition(Ogre::Vector3(_nPosition[gmtl::Xelt], _nPosition[gmtl::Yelt], _nPosition[gmtl::Zelt]));
        pRibbonTrail->addNode(pTranslatedNode);
		pRibbonTrail->setColourChange(0, 0.5, 0.5, 0.5, 0.5);
        pRibbonTrail->setInitialWidth(0, 5.0);
        pRibbonTrail->setWidthChange(0, -15.0);
    }
}

void GraphicManager::addObjectRibbonTrail(const char *_sSceneManagerName, const char *_sRibbonTrailName, const char *_sObjectName)
{
    Ogre::SceneManager *pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);
    Ogre::RibbonTrail *pRibbonTrail;

    if (pSceneManager) {
        pRibbonTrail = pSceneManager->getRibbonTrail(_sRibbonTrailName);

	    pRibbonTrail->setColourChange(0, 0.5, 0.5, 0.5, 1.0);
        pRibbonTrail->setWidthChange(0, 5.0);
    }
}

void GraphicManager::setRibbonTrailAttributes(const char *_sSceneManagerName, const char *_sRibbonTrailName, const char *_sObjectName, const float _nColorR, const float _nColorG, const float _nColorB, const float _nWidth)
{
    Ogre::SceneManager *pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);
    Ogre::RibbonTrail *pRibbonTrail;

    if (pSceneManager) {
        pRibbonTrail = pSceneManager->getRibbonTrail(_sRibbonTrailName);

        pRibbonTrail->setInitialColour(0, _nColorR, _nColorG, _nColorB);
        pRibbonTrail->setInitialWidth(0, _nWidth);
    }
}

void GraphicManager::destroyRibbonTrail(const char *_sSceneManagerName, const char *_sRibbonTrailName)
{
    Ogre::SceneManager *pSceneManager = m_pRoot->getSceneManager(_sSceneManagerName);

    if (pSceneManager) {
        pSceneManager->destroyRibbonTrail(_sRibbonTrailName);
    }
}

float GraphicManager::getAverageFPS() const
{
    return getRenderWindow()->getAverageFPS();
}

void GraphicManager::doPicking(const char *sSceneManagerName, const char *sCameraName, const unsigned int _nQueryMask, gmtl::Vec4f _nWorldRect, gmtl::Vec2f _nScreenWidth)
{
    Ogre::SceneManager  *pSceneManager;
    Ogre::Camera        *pCamera;
    Ogre::Ray           uMouseRay;
    Ogre::RaySceneQuery *uRaySceneQuery;
    float               nLeft, nRight, nTop, nBottom, nTemp;

    pSceneManager = m_pRoot->getSceneManager(sSceneManagerName);
    if (!pSceneManager) {
        f_printf("[GraphicManager] Error: Could not find scenemanager of name %s\n", sSceneManagerName);
        return;
    }

    pCamera = pSceneManager->getCamera(sCameraName);
    if (!pCamera) {
        f_printf("[GraphicManager] Error: Could not find camera of name %s\n", sCameraName);
        return;
    }

    // Grab our selection box boundaries
    f_printf("[GraphicManager] DEBUG: Picking (Screen = %f, %f), (Start = %f, %f), (Stop = %f, %f)\n", _nScreenWidth[0], _nScreenWidth[1], _nWorldRect[0], _nWorldRect[1], _nWorldRect[2], _nWorldRect[3]);
    nLeft = _nWorldRect[0] / _nScreenWidth[0];
    nTop = _nWorldRect[1] / _nScreenWidth[1];
    nRight = _nWorldRect[2] / _nScreenWidth[0];
    nBottom = _nWorldRect[3] / _nScreenWidth[1];

    // Swap top/bottom and/or left/right if the selection box is backwards
    if (nLeft > nRight) {
        nTemp = nLeft;
        nLeft = nRight;
        nRight = nTemp;
    }
    if (nTop > nBottom) {
        nTemp = nTop;
        nTop = nBottom;
        nBottom = nTemp;
    }

    // Check if the selection box is too small
    if ((nRight - nLeft) * (nBottom - nTop) <= 0.0001) {
        // Single selection, work on it
        uMouseRay = pCamera->getCameraToViewportRay(nLeft, nTop);
        uRaySceneQuery = pSceneManager->createRayQuery(uMouseRay);
        uRaySceneQuery->setRay(uMouseRay);
        uRaySceneQuery->setSortByDistance(true);
        uRaySceneQuery->setQueryMask(_nQueryMask);
        uRaySceneQuery->setQueryTypeMask(Ogre::SceneManager::ENTITY_TYPE_MASK);
        
        Ogre::RaySceneQueryResult &result = uRaySceneQuery->execute();
        Ogre::RaySceneQueryResult::iterator itr;
        
        m_uSelectedObjectList.clear();
        for (itr = result.begin(); itr != result.end(); itr++) {
            if (itr->movable) {
                Ogre::Vector3 nIntersectionPoint = uMouseRay.getPoint(itr->distance);
                //f_printf("Intersection Point: %f %f %f\n", nIntersectionPoint.x, nIntersectionPoint.y, nIntersectionPoint.z);
    
                SelectionObject *pObject = (SelectionObject*)(itr->movable);
    
                GraphicManager::getSingleton().callbackPick(pObject, gmtl::Vec3f(nIntersectionPoint.x, nIntersectionPoint.y, nIntersectionPoint.z));
                break;
            }
        }
    } else {
        // Box selection, work on it
        Ogre::PlaneBoundedVolume vol = pCamera->getCameraToViewportBoxVolume(nLeft, nTop, nRight, nBottom, true);
        Ogre::PlaneBoundedVolumeList volList;
        volList.push_back(vol);
    
        m_pVolQuery->setVolumes(volList);
        m_pVolQuery->setQueryMask(_nQueryMask);
        m_pVolQuery->setQueryTypeMask(Ogre::SceneManager::ENTITY_TYPE_MASK);
        Ogre::SceneQueryResult &uResult = m_pVolQuery->execute();
    
        m_uSelectedObjectList.clear();
        Ogre::SceneQueryResultMovableList::iterator itr;
    
        for (itr = uResult.movables.begin(); itr != uResult.movables.end(); itr++) {
            Ogre::Vector3 nIntersectionPoint = Ogre::Vector3(0, 0, 0);
            //f_printf("Intersection Point: %f %f %f\n", nIntersectionPoint.x, nIntersectionPoint.y, nIntersectionPoint.z);

            SelectionObject *pObject = (SelectionObject*)(*itr);

            GraphicManager::getSingleton().callbackPick(pObject, gmtl::Vec3f(nIntersectionPoint.x, nIntersectionPoint.y, nIntersectionPoint.z));
        }
    }
}

char* GraphicManager::consumeSelectedObjectName()
{
    vector<SelectionObject *>::iterator itrVec;

    for (itrVec = m_uSelectedObjectList.begin(); itrVec != m_uSelectedObjectList.end(); itrVec++) {
        char *pObjectName = (*itrVec)->getName();
        m_uSelectedObjectList.erase(itrVec);

        return pObjectName;
    }

    return NULL;
}

void GraphicManager::callbackPick(SelectionObject *_pObject, gmtl::Point3f _nWorldLocation)
{
    Ogre::String sName = _pObject->getName();
    unsigned int nObjectLenth = (unsigned int)strlen(sName.c_str());
    unsigned int nMessageSize = nObjectLenth + 13;

    m_uSelectedObjectList.push_back(_pObject);

    GraphicManager::getSingleton().Channel_MessageCreate(nMessageSize);      // 0-31: Name, 32-35: LocationX, 36-39: LocationY, 40-43: LocationZ
    GraphicManager::getSingleton().Channel_MessageHeader(0x0000);
    GraphicManager::getSingleton().Channel_MessageData(sName.c_str(), nObjectLenth);
    GraphicManager::getSingleton().Channel_MessageData((char*)&_nWorldLocation[0], 4);
    GraphicManager::getSingleton().Channel_MessageData((char*)&_nWorldLocation[1], 4);
    GraphicManager::getSingleton().Channel_MessageData((char*)&_nWorldLocation[2], 4);
    GraphicManager::getSingleton().Channel_Send("GRAPHICS_SELECTION");
}

void* GraphicManager::doChannelReceive(unsigned long _nChannelID, unsigned short _nHeader, void *_pData, unsigned int _nSize)
{
    return NULL;
}

#endif  // S_ENABLE_GRAPHIC
