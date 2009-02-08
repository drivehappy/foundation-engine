// S_ENABLE_GRAPHIC is defined in premake.lua, it should only be altered there
// to allow for easy stripping/adding of libraries actually needed to be compiled
#if defined(S_ENABLE_GRAPHIC)

#if !defined(_PY__F_GRAPHICMANAGER_H_)
#define _PY__F_GRAPHICMANAGER_H_

// Boost
#include <boost/python.hpp>
using namespace boost::python;

// Foundation
#include "foundation.h"

#include "py_gmtl.h"


namespace Foundation {

    namespace Graphic {

        /** Class that provides a python wrapper to the camera object.
        */
        class Camera_PythonWrapper : public Camera 
        {
        public:
            Camera_PythonWrapper(const char *_sName, SceneManager *_pSceneManager);

            gmtlVector3_PythonWrapper getPosition() const;

            gmtlQuaternion_PythonWrapper getRotation() const;

            bool isObjectVisible(gmtlVector3_PythonWrapper _nPosition) const;

			/*
            void setAspectRatio(const float _nRatio);
			*/
        };

        /** Class taht provides a python wrapper to the RenderWindow.
        */
        class RenderWindow_PythonWrapper : public RenderWindow
        {
        public:
            unsigned int getWidth() const;
            unsigned int getHeight() const;
        };

        /** Class that provides a python wrapper to the GraphicManager.
        */
        class GraphicManager_PythonWrapper : public GraphicManager
        {
        public:
            GraphicManager_PythonWrapper();
            ~GraphicManager_PythonWrapper();

            /** Return our wrapped task.
            */
            Task *getTaskRender() const;

            /**
            */
            bool initialize(const char *_sWindowTitle);
        
            /** Show or hide the cursor.
                @param
                    True to show the cursor, False to hide the cursor.
            */
            void showCursor(const bool _bShowCursor);

            /** Create a new light.
            */
            void createLight(const char *_sSceneManagerName, const char *_sLightName, const gmtl::Vec3f _nPosition, const gmtl::Vec3f _nDiffuse, const gmtl::Vec3f _nSpecular);

            /**
            */
            void clearBloomCompositors();

            /**
            */
            void enableBloom(const bool _bEnabled);

            /**
            */
            void setShadowTechnique(const char *_sSceneManagerName);

            /**
            */
            void destroy();

            /**
            */
            Camera_PythonWrapper* addCamera(const char *_sSceneManagerName, const char *_sCameraName, const int _nZOrder, const float _nViewportLeft, const float _nViewportTop, const float _nViewportWidth, const float _nViewportHeight);

            /**
            */
            Camera_PythonWrapper* getCamera(const char *_sSceneManagerName, const char *_sCameraName) const;

            /**
            */
            RenderWindow_PythonWrapper* getRenderWindow() const;

            /** Python wrapper to create a line object.
                @param
                    String ID of the SceneManager that will create the line.
                @param
                    String ID of the line to create.
                @param
                    The vector of line points as a source.
                */
            void addLine(const char *_sSceneManagerName, const char *_sID, list _nPoints, tuple _nColor);
            /** Python wrapper to update a line object.
                @param
                    String ID of the SceneManager that will update the line.
                @param
                    String ID of the line to update.
                @param
                    The vector of line points as a source.
            */
            void updateLine(const char *_sSceneManagerName, const char *_sID, list _nPoints);

            /**
            */
            void doPicking(const char *sSceneManagerName, const char *sCameraName, const unsigned int _nQueryMask, gmtlVector4_PythonWrapper _nWorldRect, gmtlVector2_PythonWrapper _nScreenWidth);
    
            /**
            */
            PyObject* consumeSelectedObjectName();

            /**
            */
            gmtlQuaternion_PythonWrapper getMeshRotation(const char *_sSceneManagerName, const char *_sID);
        };

    };

};

#endif  // _PY__F_GRAPHICMANAGER_H_

#endif  // S_ENABLE_GRAPHIC
