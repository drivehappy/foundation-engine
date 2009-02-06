// S_ENABLE_GRAPHIC is defined in premake.lua, it should only be altered there
// to allow for easy stripping/adding of libraries actually needed to be compiled
#if defined(S_ENABLE_GRAPHIC)

#if !defined(_PY__F_GUISYSTEM_H_)
#define _PY__F_GUISYSTEM_H_

// Foundation
#include "graphics/guiSystem.h"

#include "py_graphicManager.h"

namespace Foundation
{
    namespace Graphic
    {
        class GUIManager_PythonWrapper : public GUIManager
        {
            public:
                GUIManager_PythonWrapper();
                ~GUIManager_PythonWrapper();
                
                /** Python wrapper to initialize the GUIManager.
                 */
                void initialize(RenderWindow_PythonWrapper *_pRenderWindow);
                /** Python wrapper to destroy the GUIManager.
                 */
                void destroy();

                /** Python wrapper to inject mouse input event.
                 */
                bool injectMouseEvent(const Input::MouseEvent & _mEvent);
                /** Python wrapper to inject keyboard input event.
                 */
                bool injectKeyboardEvent(const Input::KeyboardEvent & _mEvent);

                /** Python wrapper to set the mouse cursor position.
                 */
                void setMousePosition(unsigned int _nX, unsigned int _nY, unsigned int _nZ);

                /** Python wrapper to create a GUI window object.
                 */
                void createWindow(const char *_sParent, const char *_sName, unsigned int _nPositionX, unsigned int _nPositionY, unsigned int _nSizeX, unsigned int _nSizeY, const char *_sCaption);
                /** Python wrapper to create a GUI button object.
                 */
                void createButton(const char *_sParent, const char *_sName, unsigned int _nPositionX, unsigned int _nPositionY, unsigned int _nSizeX, unsigned int _nSizeY, const char *_sCaption);
                /** Python wrapper to create a GUI static text object.
                 */
                void createStaticText(const char *_sParent, const char *_sName, unsigned int _nPositionX, unsigned int _nPositionY, unsigned int _nSizeX, unsigned int _nSizeY, const char *_sCaption);

                /**
                 */
                void setObjectColor(const char *_sObjectName, float _nR, float _nG, float _nB);

                /**
                 */
                void setTextFont(const char *_sObjectName, const char *_sFontFace, int _nFontSize);

                /** Destroy a GUI object.
                 */
                void destroyObject(const char *_sName);

                /** Python wrapper to set the caption of a widget.
                 */
                void setWidgetCaption(const char *_sName, const char *_sCaption);

                /**
                 */
                void setActiveViewport(const unsigned short _nViewport);

            private:
                
        };
    };
};

#endif  // _PY__F_GUISYSTEM_H_

#endif  // S_ENABLE_GRAPHIC
