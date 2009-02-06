// S_ENABLE_GRAPHIC is defined in premake.lua, it should only be altered there
// to allow for easy stripping/adding of libraries actually needed to be compiled
#if defined(S_ENABLE_GRAPHIC)

#if !defined(__F_WATER_H_)
#define __F_WATER_H_

// Foundation
#include "graphics/gfxSystem.h"

// Ogre
#include <Ogre.h>

namespace Foundation
{
    namespace Graphic {

        /** Interface class to encapsulate different water rendering libraries.
        */
        class Water {
        public:
            /** Default constructor.
            */
            Water();
            /** Destructor
            */
            ~Water();

            /** Initialize our water, pass in our graphics manager.
            */
            void initialize(const char *_sSceneManager, const char *_sCamera);
            /** Destroy our water.
            */
            void destroy();

            // Test funcs
            void createScene();
            void configureShader();

            // Tasks
            Task *taskRender;
            Task *getTaskRender() const {
                return taskRender;
            }

            inline static void doTaskRender_Wrapper(void *_class, void *_args) {
                reinterpret_cast<Water*>(_class)->doTaskRender(_args);
            };

        private:
            // Tasks
            void                        *doTaskRender(void*);

            // Graphics helpers
            Ogre::Camera                *mCamera;
            Ogre::SceneManager          *mSceneMgr;
            Ogre::Root                  *mRoot;
            Ogre::SceneNode             *mMainNode;

            Ogre::RenderWindow          *mWindow;
            Ogre::Entity                *mCurrentEntity;
            Ogre::Entity                *mOceanSurfaceEnt;

            size_t                      mCurrentMaterial;
            Ogre::MaterialPtr           mActiveMaterial;
            Ogre::Pass                  *mActivePass;
            Ogre::GpuProgramPtr         mActiveFragmentProgram;
            Ogre::GpuProgramPtr         mActiveVertexProgram;
            Ogre::GpuProgramParametersSharedPtr mActiveFragmentParameters;
            Ogre::GpuProgramParametersSharedPtr mActiveVertexParameters;
        };

    };
};

#endif  // __F_WATER_H_

#endif  // S_ENABLE_GRAPHIC
