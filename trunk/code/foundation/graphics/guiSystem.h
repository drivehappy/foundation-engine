// S_ENABLE_GRAPHIC is defined in premake.lua, it should only be altered there
// to allow for easy stripping/adding of libraries actually needed to be compiled
#if defined(S_ENABLE_GRAPHIC)

// Header guard
#if !defined(__F_GUISYSTEM_H_)
#define __F_GUISYSTEM_H_

// Foundation
#include "kernel/kernel.h"
#include "input/inputSystem.h"
#include "graphics/gfxSystem.h"

// MyGUI
#include <MyGUI.h>


namespace Foundation
{
    namespace Graphic
    {
        /**
         */
        enum MouseEvent
        {
            PRESSED = 1,
            RELEASED,
            MOVE,
        };

        /** Class that provides an abstract interface to a widget.
         */
        class GUIWidget : public MyGUI::Widget
        {
            public:
                GUIWidget();
                ~GUIWidget();
        };

        /** Class that provides a GUI interface.
         */
        class GUIManager
        {
            public:
                /** Retrieve the singleton class instance.
                 */
                static GUIManager& getSingleton();

                /** Initialize the GUIManager.
                 */
                static void initialize(RenderWindow *_pRenderWindow);
                /** Destroy the GUIManager.
                 */
                static void destroy();

                
                /** Handle mouse button down events.
                 */
                static void eventMouseButtonPressed(MyGUI::WidgetPtr _pSender, int _nX, int _nY, MyGUI::MouseButton _nId);

                /** Handle mouse button up events.
                 */
                static void eventMouseButtonReleased(MyGUI::WidgetPtr _pSender, int _nX, int _nY, MyGUI::MouseButton _nId);

                /** Handle mouse move events.
                 */
                static void eventMouseMove(MyGUI::WidgetPtr _pSender, int _nX, int _nY);


                /** Inject mouse input event.
                 */
                static bool injectMouseEvent(const Input::MouseEvent & _mEvent);
                /** Inject keyboard input event.
                 */
                static bool injectKeyboardEvent(const Input::KeyboardEvent & _mEvent);

                /** Set the mouse cursor position.
                 */
                static void setMousePosition(unsigned int _nX, unsigned int _nY, unsigned int _nZ);

                /** Create a GUI window object.
                 */
                static void createWindow(const char *_sParent, const char *_sName, unsigned int _nPositionX, unsigned int _nPositionY, unsigned int _nSizeX, unsigned int _nSizeY, const char *_sCaption);
                /** Create a GUI button object.
                 */
                static void createButton(const char *_sParent, const char *_sName, unsigned int _nPositionX, unsigned int _nPositionY, unsigned int _nSizeX, unsigned int _nSizeY, const char *_sCaption);
                /** Create a GUI static text object.
                 */
                static void createStaticText(const char *_sParent, const char *_sName, unsigned int _nPositionX, unsigned int _nPositionY, unsigned int _nSizeX, unsigned int _nSizeY, const char *_sCaption);

                /**
                 */
                static void setObjectColor(const char *_sObjectName, float _nR, float _nG, float _nB);

                /**
                 */
                static void setTextFont(const char *_sObjectName, const char *_sFontFace, int _nFontSize);

                /** Desotry a GUI object.
                 */
                static void destroyObject(const char *_sName);

                /** Set caption of a widget.
                 */
                static void setWidgetCaption(const char *_sName, const char *_sCaption);

                /**
                 */
                static void setActiveViewport(const unsigned short _nViewport);

                /**
                 */
                static void setWidgetVisible(const char *_sName, const bool _bVisible);

            protected:
                GUIManager();
                GUIManager(const GUIManager&);
                GUIManager& operator=(const GUIManager&);
                ~GUIManager();

            private:
                static MyGUI::Gui           *m_pGUI;
                static Channel              *m_pMouseEventChannel;
        };
    };
};

#endif  //__F_GUISYSTEM_H_

#endif  // S_ENABLE_GRAPHIC
