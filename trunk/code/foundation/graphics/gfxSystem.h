// S_ENABLE_GRAPHIC is defined in premake.lua, it should only be altered there
// to allow for easy stripping/adding of libraries actually needed to be compiled
#if defined(S_ENABLE_GRAPHIC)

#if !defined(__F_GRAPHICMANAGER_H_)
#define __F_GRAPHICMANAGER_H_

// Foundation
#include "kernel/kernel.h"

// OGRE
#include <Ogre.h>

// STL
#include <vector>
using std::vector;


namespace Foundation
{
    namespace Graphic
    {
        /** Class that provides objects for selection.
        */
        class SelectionObject : public Ogre::MovableObject
        {
            public:
                char* getName();

            private:

        };

        /** Used from Intermediate Tut 04 from Ogre.
            http://www.ogre3d.org/wiki/index.php/Intermediate_Tutorial_4#PlaneBoundedVolumeListSceneQuery
         */
        class SelectionRectangle : public Ogre::ManualObject {
        public:
            SelectionRectangle(const Ogre::String &name)
                : Ogre::ManualObject(name)
            {
            }
        
            /**
            * Sets the corners of the SelectionRectangle.  Every parameter should be in the
            * range [0, 1] representing a percentage of the screen the SelectionRectangle
            * should take up.
            */
            void setCorners(float left, float top, float right, float bottom)
            {
            }
        
            void setCorners(const Ogre::Vector2 &topLeft, const Ogre::Vector2 &bottomRight)
            {
                setCorners(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
            }
        };


        /** Class that provides an interface to a scenemanager object.
        */
        class SceneManager : public Ogre::SceneManager
        {
            public:
                SceneManager(const char *_sSceneManagerName)
                    : Ogre::SceneManager(_sSceneManagerName) {};
                SceneManager(const SceneManager & _sSceneManager)
                    : Ogre::SceneManager(_sSceneManager) {};
                ~SceneManager() {};

            private:
        };

        /** Class that provides an interface to a camera object.
        */
        class Camera : public Ogre::Camera
        {
            public:
                Camera(const char *_sName, SceneManager *_pSceneManager)
                    : Ogre::Camera(_sName, _pSceneManager) 
                    {};
                ~Camera() {};

                void setPosition(gmtl::Vec3f _nPosition) {
                    Ogre::Camera::setPosition(Ogre::Vector3(_nPosition[gmtl::Xelt], _nPosition[gmtl::Yelt], _nPosition[gmtl::Zelt]));
                };
                void setLookAt(gmtl::Vec3f _nPosition) {
                    Ogre::Camera::lookAt(Ogre::Vector3(_nPosition[gmtl::Xelt], _nPosition[gmtl::Yelt], _nPosition[gmtl::Zelt]));
                }
                void setRotation(gmtl::Quatf _nRotation) {
                    Ogre::Camera::setOrientation(Ogre::Quaternion(_nRotation[gmtl::Welt], _nRotation[gmtl::Xelt], _nRotation[gmtl::Yelt], _nRotation[gmtl::Zelt]));
                };
                void yaw(float _nDegree) {
                    Ogre::Camera::yaw(Ogre::Angle(_nDegree));
                };
                void pitch(float _nDegree) {
                    Ogre::Camera::pitch(Ogre::Angle(_nDegree));
                };
    
                gmtl::Vec3f getPosition() const {
                    return gmtl::Vec3f(Ogre::Camera::getPosition().x, Ogre::Camera::getPosition().y, Ogre::Camera::getPosition().z);
                };

                void moveRelative(gmtl::Vec3f _nPosition) {
                    Ogre::Camera::moveRelative(Ogre::Vector3(_nPosition[0], _nPosition[1], _nPosition[2]));
                };

                bool isObjectVisible(const gmtl::Vec3f _nPosition) const {
                    return Ogre::Camera::isVisible(Ogre::Vector3(_nPosition[0], _nPosition[1], _nPosition[2]));
                };

                gmtl::Quatf getRotation() const {
                    Ogre::Quaternion nRotation = Ogre::Camera::getOrientation();

                    return gmtl::Quatf(nRotation[0], nRotation[1], nRotation[2], nRotation[3]);
                }

				/*
                void setAspectRatio(const float _nRatio) {
                    Ogre::Camera::setAspectRatio(_nRatio);
                };
				*/

            private:
                
        };

        /** Class that provides an interface to a renderwindow object.
        */
        class RenderWindow : public Ogre::RenderWindow
        {
            public:
                RenderWindow()
                    : Ogre::RenderWindow() {};
                ~RenderWindow() {};

                unsigned int getWidth() const {
                    return Ogre::RenderWindow::getWidth();
                };

                unsigned int getHeight() const {
                    return Ogre::RenderWindow::getHeight();
                };

            private:
        };

        /** Class provides a clean interface to different underlying graphic systems.
            @note
                If adding a new graphics library, that libraries interface should be
                exposed <b>only</b> inside the GraphicManager.
            @par
                Maintenence:
                If more functionality is needed for the engine the GraphicManager's
                interface should be updated and wrapped around any underlying new functions
                within a library.
         */
        class GraphicManager : public Channel {
            public:
                /** Retrieve the singleton class instance.
                */
                static GraphicManager& getSingleton();
            
                /** Initialize the GraphicManager.
                */
                static bool initialize(const char *_sWindowTitle);
                /** Destroy the GraphicManager.
                */
                static void destroy();
                /** Configure the GraphicManager.
                */
                static bool configure(const char *_sWindowTitle);
                /** Setup the resource files and locations.
                */
                static void setupResources();
                /** Load the resource files after setting locations up.
                */
                static void loadResources();
                /** Create a bare scene.
                */
                static void createScene();

                /** Create a new camera.
                    @param
                        A unique string for the camera name.
                */
                static void createCamera(const char _sCameraName);

                /** Show or hide the cursor.
                    @param
                        True to show the cursor, False to hide the cursor.
                */
                static void showCursor(const bool _bShowCursor);

                /** Create a new light.
                */
                static void createLight(const char *_sSceneManagerName, const char *_sLightName, const gmtl::Vec3f _nPosition, const gmtl::Vec3f _nDiffuse, const gmtl::Vec3f _nSpecular);

                /** Set shadow technique.
                */
                static void setShadowTechnique(const char *_sSceneManagerName);

                /** Clear compositors for viewports.
                */
                static void clearBloomCompositors();

                /** Enable/Disable bloom.
                */
                static void enableBloom(const bool _bEnabled);

                // Tasks
                /** Task wrapper for rendering the scene.
                    @param
                        The GraphicManager to use, since this is a singleton, the singleton
                        instance can be passed in.
                    @param
                        The argument to pass in, generally the elapsed time.
                */
                inline static void doTaskRender_Wrapper(void *_pClass, void *_pArgs)
                    { reinterpret_cast<GraphicManager*>(_pClass)->doTaskRender(_pArgs); };
                /** Retrieve our task, useful for exposing the class to Python.
                */
                Task *getTask() const       { return m_pTask; };

				// ParticleSystem
				/** Add a new particle system.
				*/
				void addParticleSystem(const char *_sSceneManagerName, const char *_sParticleSystemName, const char *_sAttachEntityName, const char *_sScriptName);
                /**
                */
                void destroyParticleSystem(const char *_sSceneManagerName, const char *_sParticleSystemName);
                /** Show or hide an emitter in a particle system.
                */
				void enableParticleSystemEmitter(const char *_sSceneManagerName, const char *_sParticleSystemName, const unsigned int _nEmitterIndex, const bool _bVisible);
				/** Show or hide a particle system.
				*/
				void showParticleSystem(const char *_sSceneManagerName, const char *_sParticleSystemName, const bool _bVisible);
				/** Translate an existing particle system.
				*/
				void translateParticleSystem(const char *_sSceneManagerName, const char *_sParticleSystemName, gmtl::Vec3f _nPosition);
				/** Rotate an existing particle system.
				*/
				void rotateParticleSystem(const char *_sSceneManagerName, const char *_sParticleSystemName, gmtl::Quatf _nRotation);
				/** Adjust the time of the particle system.
				*/
				void fastForwardParticleSystem(const char *_sSceneManagerName, const char *_sParticleSystemName, float _nTime);

                // Camera
                /** Add a new camera.
                    @param
                        String ID of the SceneManager containing the camera.
                    @param
                        String ID of the camera name.
                */
                Camera* addCamera(const char *_sSceneManagerName, const char *_sCameraName, const int _nZOrder, const float _nViewportLeft, const float _nViewportTop, const float _nViewportWidth, const float _nViewportHeight);
                /** Remove an existing camera.
                    @param
                        String ID of the SceneManager containing the camera.
                    @param
                        String ID of the camera name.
                */
                void removeCamera(const char *_sSceneManagerName, const char *_sCameraName);
                /** Set the camera position.
                    @param
                        String ID of the SceneManager containing the camera.
                    @param
                        A pointer to the camera object.
                    @param
                        The position to set the camera to.
                */
                void setCameraPosition(const char *_sSceneManagerName, const char *_sCameraName, gmtl::Vec3f _nPosition);
                /** Get the camera position.
                    @param
                        String ID of the SceneManager containing the camera.
                    @param
                        A pointer to the camera object.
                */
                gmtl::Vec3f getCameraPosition(const char *_sSceneManagerName, const char *_sCameraName) const;
                /** Set the camera rotation.
                    @param
                        String ID of the SceneManager containing the camera.
                    @param
                        A pointer to the camera object.
                    @param
                        The rotation to set the camera to.
                */
                void setCameraRotation(const char *_sSceneManagerName, const char *_sCameraName, gmtl::Vec3f _nAxis, float _nDegreeAngle);
                /**
                */
                void setCameraDirection(const char *_sSceneManagerName, const char *_sCameraName, gmtl::Vec3f _nDirection);
                /**
                */
                void setCameraTracking(const char *_sSceneManagerName, const char *_sCameraName, const bool _bEnabled, const char *_sObjectTrack, const gmtl::Vec3f _nOffset);
                /** Set the camera rotation to look at a point.
                    @param
                        String ID of the scenemanager holding the camera.
                    @param
                        String ID of the camera to rotate.
                    @param
                        A 3d point to look at.
                */
                void setCameraLookat(const char *_sSceneManagerName, const char *_sCameraName, gmtl::Vec3f _nPosition);
                /** Get the camera rotation.
                    @param
                        String ID of the SceneManager containing the camera.
                    @param
                        A pointer to the camera object.
                */
                gmtl::Quatf getCameraRotation(const char *_sSceneManagerName, const char *_sCameraName) const;
                /** Return a specified camera.
                    @param
                        The scene manager that contains the camera.
                    @param
                        String ID of the camera to retrieve.
                */
                Camera* getCamera(const char *_sSceneManagerName, const char *_sCameraName) const;

                // Scene Nodes
                /** Add a new mesh to the scene.
                    @param
                        String ID of the SceneManager containing the entity.
                    @param
                        String ID of the mesh to add.
                    @param
                        The filename of the mesh. Paths are not needed if resource locations
                        are specified correctly in resources.cfg.
                */
                void addMesh(const char *_sSceneManagerName, const char *_sID, const char *_sMeshFilename, unsigned int _nQueryFlags);
                /**
                */
                void removeMesh(const char *_sSceneManagerName, const char *_sID);
                /** Set a mesh's position.
                    @param
                        String ID of the SceneManager containing the entity.
                    @param
                        String ID of the mesh to change.
                    @param
                        The position in which to set the mesh.
                */
                void setMeshPosition(const char *_sSceneManagerName, const char *_sID, gmtl::Vec3f _nPosition);
                /** Get a mesh's rotation.
                    @param
                        String ID of the SceneManager containing the entity.
                    @param
                        String ID of the mesh.
                */
                gmtl::Quatf getMeshRotation(const char *_sSceneManagerName, const char *_sID);
                /** Set a mesh's rotation.
                    @param
                        String ID of the SceneManager containing the entity.
                    @param
                        String ID of the mesh to change.
                    @param
                        The rotation in which to set the mesh.
                */
                void setMeshRotation(const char *_sSceneManagerName, const char *_sID, gmtl::Quatf _nRotation);
                /** Set a mesh's rotation.
                    @param
                        String ID of the SceneManager containing the entity.
                    @param
                        String ID of the mesh to change.
                    @param
                        The rotation in which to set the mesh.
                    @param
                        The angle of rotation.
                */
                void setMeshRotation(const char *_sSceneManagerName, const char *_sID, const gmtl::Vec3f _nAxis, const float _nDegreeAngle);
                /** Set a mesh's scale.
                    @param
                        String ID of the SceneManager containing the entity.
                    @param
                        String ID of the mesh to change.
                    @param
                        The scale in which to set the mesh.
                */
                void setMeshScale(const char *_sSceneManagerName, const char *_sID, gmtl::Vec3f _nScale);
                /** Set a mesh's material.
                    @param
                        String ID of the SceneManager containing the entity.
                    @param
                        String ID of the mesh.
                    @param
                        The material to apply to the mesh.
                */
                void setMeshMaterial(const char *_sSceneManagerName, const char *_sID, const char *_sMaterialName);
                /** Set a mesh's visibility.
                    @param
                        String ID of the SceneManager containing the entity.
                    @param
                        String ID of the mesh.
                    @param
                        Boolean representing visibility.
                */
                void setMeshVisibility(const char *_sSceneManagerName, const char *_sID, const bool _bVisible);

                /**
                */
                void destroyAllEntities(const char *_sSceneManagerName);

                /**
                */
                void setSkybox(const char *_sSceneManagerName, const char *_sSkyboxName);

                // Lines
                /** Create a line object.
                    @param
                        String ID of the SceneManager that will create the line.
                    @param
                        String ID of the line to create.
                    @param
                        The vector of line points as a source.
                */
                void addLine(const char *_sSceneManagerName, const char *_sID, vector<gmtl::Vec3f> _nPoints, float _nRed, float _nGreen, float _nBlue);
                /** Clear a line object.
                    @param
                        String ID of the SceneManager that contains the line to clear.
                    @param
                        String ID of the line to clear.
                */
                void clearLine(const char *_sSceneManagerName, const char *_sID);
                /** Destroy a line object.
                    @param
                        String ID of the SceneManager that contains the line to destroy.
                    @param
                        String ID of the line to destroy.
                */
                void destroyLine(const char *_sSceneManagerName, const char *_sID);
                /** Update a line object.
                    @param
                        String ID of the SceneManager that contains the line to update.
                    @param
                        String ID of the line to update.
                    @param
                        The vector of line points as a source.
                */
                void updateLine(const char *_sSceneManagerName, const char *_sID, vector<gmtl::Vec3f> _nPoints);

                /** Create a plane object.
                */
                void addPlane(const char *_sSceneManagerName, const char *_sID, float _nWidth, float _nHeight, unsigned int _nSegmentsX, unsigned int _nSegmentsY);

                // Scene Managers
                /** Add a new scenemanager to the graphicmanager.
                    @param
                        String ID of the scene manager's name.
                */
                void addSceneManager(const char *_sSceneManagerName);
                /** Remove an existing scenemanager from the graphicmanager.
                    @param
                        String ID of the scene manager's name.
                */
                void removeSceneManager(const char *_sSceneManagerName);
                /** Return a scene manager.
                    @param
                        String ID of the scene manager to retrieve.
                */
                SceneManager* getSceneManager(const char *_sSceneManagerName) const;

                // Window
                /** Return the render window.
                */
                RenderWindow* getRenderWindow() const;
                /** Return the render window handle.
                */
                size_t getWindowHandle() const;

                /** Create a ribbon trail.
                */
                void createRibbonTrail(const char *_sSceneManagerName, const char *_sObjectAttachName, const char *_sRibbonTrailName, const char *_sMaterial, const gmtl::Vec3f _nPosition);
                /** Add a ribbon trail to a node
                */
                void addObjectRibbonTrail(const char *_sSceneManagerName, const char *_sRibbonTrailName, const char *_sObjectName);
                /** Set the ribbon trail color
                */
                void setRibbonTrailAttributes(const char *_sSceneManagerName, const char *_sRibbonTrailName, const char *_sObjectName, const float _nColorR, const float _nColorG, const float _nColorB, const float _nWidth);
                /**
                */
                void destroyRibbonTrail(const char *_sSceneManagerName, const char *_sRibbonTrailName);

                // Stats
                /** Return the average FPS.
                */
                float getAverageFPS() const;

                /** A channel event for the receiving data.
                    @param
                        The graphic manager that does the receiving.
                    @param
                        Arguments to the task.
                */
                inline static void doChannelReceive_Wrapper(void *_class, unsigned long _nChannelID, unsigned short _nHeader, char *_pData, unsigned int _nSize) {
                    reinterpret_cast<GraphicManager*>(_class)->doChannelReceive(_nChannelID, _nHeader, _pData, _nSize);
                };

                /**
                 */
                static void doPicking(const char *sSceneManagerName, const char *sCameraName, const unsigned int _nQueryMask, gmtl::Vec4f _nWorldRect, gmtl::Vec2f _nScreenWidth);

                /**
                 */
                static char* consumeSelectedObjectName();
    
            protected:
                GraphicManager();
                GraphicManager(const GraphicManager&);
                GraphicManager& operator=(const GraphicManager&);
                ~GraphicManager();
            
            private:
                // Tasks
                static Task                 *m_pTask;
                static void                 *doTaskRender(void *_pArgs);

                // OGRE
                static void                 callbackPick(SelectionObject *_pObject, gmtl::Point3f _nWorldLocation);
                static void                 *doChannelReceive(unsigned long _nChannelID, unsigned short _nHeader, void *_pData, unsigned int _nSize);

                static Ogre::Root                       *m_pRoot;
                static RenderWindow                     *m_pWindow;
                static Ogre::String                     m_sResourcePath;
                static Ogre::RaySceneQuery              *m_pRaySceneQuery;
                static SelectionRectangle               *m_pRectSelection;
                static Ogre::PlaneBoundedVolumeListSceneQuery   *m_pVolQuery;
                
                static vector<Ogre::SceneManager *>     m_uSceneManagerList;
                static vector<Ogre::Camera *>           m_uCameraList;
                static vector<SelectionObject *>        m_uSelectedObjectList;
        };

    };
};

#endif  // __F_GRAPHICMANAGER_H_

#endif  // S_ENABLE_GRAPHIC

