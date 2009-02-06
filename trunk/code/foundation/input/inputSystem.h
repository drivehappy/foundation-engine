// S_ENABLE_INPUT is defined in premake.lua, it should only be altered there
// to allow for easy stripping/adding of libraries actually needed to be compiled
#if defined(S_ENABLE_INPUT)

#if !defined(__F_INPUT_SYSTEM_H_)
#define __F_INPUT_SYSTEM_H_

// Foundation
#include "kernel/kernel.h"

// OIS
#include "OIS.h"

// Include these for the ostringstream in Initialize()
#include <string>
#include <sstream>
#include <queue>
using std::queue;
#include <vector>
using std::vector;

namespace Foundation
{
    namespace Input
    {
        /** Enum provides abstract events that are used in each event to save
            more specific attributes otherwise lost
         */
        enum Events {   MOUSE_MOVE = 0x0, MOUSE_BUTTON_PRESSED, MOUSE_BUTTON_RELEASED,
                        KEYBOARD_KEY_PRESSED = 0x10, KEYBOARD_KEY_RELEASED,
                        JOYSTICK_MOVE = 0x20, JOYSTICK_BUTTON_PRESSED, JOYSTICK_BUTTON_RELEASED };

        /** Enum provides abstraction of keycodes for various input libraries.
            Currently maps directly to OIS keycodes.
         */
        enum Keycode {
            KC_UNASSIGNED = 0x0,
            KC_ESCAPE,
            KC_1,
            KC_2,
            KC_3,
            KC_4,
            KC_5,
            KC_6,
            KC_7,
            KC_8,
            KC_9,
            KC_0,

            KC_NUMPAD7     = OIS::KC_NUMPAD7,
            KC_NUMPAD8     = OIS::KC_NUMPAD8,
            KC_NUMPAD9     = OIS::KC_NUMPAD9,
            KC_SUBTRACT    = OIS::KC_SUBTRACT,
            KC_NUMPAD4     = OIS::KC_NUMPAD4,
            KC_NUMPAD5     = OIS::KC_NUMPAD5,
            KC_NUMPAD6     = OIS::KC_NUMPAD6,
            KC_ADD         = OIS::KC_ADD,
            KC_NUMPAD1     = OIS::KC_NUMPAD1,
            KC_NUMPAD2     = OIS::KC_NUMPAD2,
            KC_NUMPAD3     = OIS::KC_NUMPAD3,
            KC_NUMPAD0     = OIS::KC_NUMPAD0,

            KC_A = OIS::KC_A,
            KC_B = OIS::KC_B,
            KC_C = OIS::KC_C,
            KC_D = OIS::KC_D,
            KC_E = OIS::KC_E,
            KC_F = OIS::KC_F,
            KC_G = OIS::KC_G,
            KC_H = OIS::KC_H,
            KC_I = OIS::KC_I,
            KC_J = OIS::KC_J,
            KC_K = OIS::KC_K,
            KC_L = OIS::KC_L,
            KC_M = OIS::KC_M,
            KC_N = OIS::KC_N,
            KC_O = OIS::KC_O,
            KC_P = OIS::KC_P,
            KC_Q = OIS::KC_Q,
            KC_R = OIS::KC_R,
            KC_S = OIS::KC_S,
            KC_T = OIS::KC_T,
            KC_U = OIS::KC_U,
            KC_V = OIS::KC_V,
            KC_W = OIS::KC_W,
            KC_X = OIS::KC_X,
            KC_Y = OIS::KC_Y,
            KC_Z = OIS::KC_Z,

            KC_SPACE = OIS::KC_SPACE,
        };

        /** Enum to provide mouse button IDs.
         */
        enum MouseButtonID {
            MB_Left = 0x0,
            MB_Right,
            MB_Middle,
            MB_Button3,
            MB_Button4,
            MB_Button5,
            MB_Button6,
            MB_Button7,
        };

        /** Struct provides abstract mouse events
         */
        struct MouseEvent : public OIS::MouseEvent
        {
            MouseEvent(const OIS::MouseEvent & event) :
                OIS::MouseEvent(event)
            {
                // placeholder
            }

            MouseButtonID m_nMouseButtonID;
            Events m_nMouseEventID;
            int m_nAxisX;
            int m_nAxisY;
        };
        
        /** Struct provides abstract key events
         */
        struct KeyboardEvent : public OIS::KeyEvent
        {
            KeyboardEvent(const OIS::KeyEvent & event) :
                OIS::KeyEvent(event)
            {
                // placeholder
            }

            Events m_nKeyboardEventID;
            int m_nValue;
        };

        /** Struct provides abstract joystick events
         */
        struct JoystickEvent : public OIS::JoyStickEvent
        {
            JoystickEvent(const OIS::JoyStickEvent & event) :
                OIS::JoyStickEvent(event)
            {
                // placeholder
            }

            int m_nJoystickIndex;
            int m_nJoystickButtonID;
            int m_nAxis;
            float m_nValue;
            Events m_nJoystickEventID;
        };

        /**
         */
        class JoyStickInputHandler : public OIS::JoyStickListener
        {
        public:
            JoyStickInputHandler(const unsigned int _nId, OIS::JoyStick *joystick)
            {
                m_nJoyStickId = _nId;
                m_pJoyStick = joystick;

                if (m_pJoyStick)
                    m_pJoyStick->setEventCallback(this);
            }

            void capture()
            {
                if (m_pJoyStick)
                    m_pJoyStick->capture();
            }
            
            queue<JoystickEvent*> getEventList()
            {
                queue<JoystickEvent*> m_BufferEvents = m_BufferJoystick;
                m_BufferJoystick = queue<JoystickEvent*>();
                return m_BufferEvents;
            }

            // JoystickListener
            bool buttonPressed(const OIS::JoyStickEvent &arg, int button)
            {
                JoystickEvent *newJoystickEvent = new JoystickEvent(arg);
                newJoystickEvent->m_nJoystickButtonID = button;
                newJoystickEvent->m_nJoystickEventID = JOYSTICK_BUTTON_PRESSED;
                m_BufferJoystick.push(newJoystickEvent);

                return true;
            }
            bool buttonReleased(const OIS::JoyStickEvent &arg, int button)
            {
                JoystickEvent *newJoystickEvent = new JoystickEvent(arg);
                newJoystickEvent->m_nJoystickButtonID = button;
                newJoystickEvent->m_nJoystickEventID = JOYSTICK_BUTTON_RELEASED;
                m_BufferJoystick.push(newJoystickEvent);

                return true;
            }
            bool axisMoved(const OIS::JoyStickEvent &arg, int axis)
            {
                vector<OIS::Axis> axes = arg.state.mAxes;
                
                vector<OIS::Axis>::iterator itrVec;
                int nAxisIndex;

                for (nAxisIndex = 0, itrVec = axes.begin(); itrVec != axes.end(); itrVec++, nAxisIndex++) {
                    JoystickEvent *newJoystickEvent = new JoystickEvent(arg);
                    newJoystickEvent->m_nJoystickIndex = m_nJoyStickId;
                    newJoystickEvent->m_nAxis = nAxisIndex;
                    newJoystickEvent->m_nJoystickEventID = JOYSTICK_MOVE;
                    newJoystickEvent->m_nValue = (float)(*itrVec).abs;
                    m_BufferJoystick.push(newJoystickEvent);
                }

                return true;
            }

        private:
            unsigned int            m_nJoyStickId;
            queue<JoystickEvent*>   m_BufferJoystick;
            OIS::JoyStick           *m_pJoyStick;
        };

        /** Base Class that provides buffered input functions.
            @note
                Utilizes OIS "Listener" classes.
         */
        class BufferedInputHandler : public OIS::KeyListener, public OIS::MouseListener
        {
            public:
                BufferedInputHandler()
                {}

                /** Initializes all the devices we may need.
                    @param
                        Keyboard device. NULL if none.
                    @param
                        Mouse device. NULL if none.
                    @param
                        Joystick device. NULL if none.
                 */
                void setupInputDevices(OIS::Keyboard *keyboard = 0, OIS::Mouse *mouse = 0)
                {
                    if (keyboard)
                        keyboard->setEventCallback(this);

                    if (mouse)
                        mouse->setEventCallback(this);
                }

                // KeyListener
                virtual bool keyPressed(const OIS::KeyEvent &arg) { return true; }
                virtual bool keyReleased(const OIS::KeyEvent &arg) { return true; }

                // MouseListener
                virtual bool mouseMoved(const OIS::MouseEvent &arg) { return true; }
                virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id) { return true; }
                virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id) { return true; }

                
        };

        /** Class provides a clean interface to different underlying input systems.
            @note
                If adding a new input library, that libraries interface should be
                exposed <b>only</b> inside the InputManager.
            @par
                Maintenence:
                If more functionality is needed for the engine the InputManager's
                interface should be updated and inherited around any underlying new functions
                within a library.
         */
        class InputManager : private BufferedInputHandler
        {
            public:
                /** Get our singleton class.
                */
                static InputManager& getSingleton();

                /** Initialize the input to a window handle.
                */
                static void initialize(size_t windowHnd = 0);

                /** Destroy and cleanup.
                */
                static void destroy();

                /** Set the Window Extents when a window resizes.
                */
                static void setWindowExtents(unsigned int _nWidth, unsigned int _nHeight);

                /**
                */
                static void setupJoystickDevice(OIS::JoyStick *joystick)
                {
                    if (joystick) {
                        JoyStickInputHandler *newJoystickHandler = new JoyStickInputHandler(m_pJoyStickList->size(), joystick);
                        m_pJoyStickList->push_back(newJoystickHandler);
                    }
                }

                /** Retrieve a buffered mouse event.
                */
                static MouseEvent* getMouseEvent()
                {
                    MouseEvent *event = NULL;

                    if (m_BufferMouse.size()) {
                        event = m_BufferMouse.front();
                        m_BufferMouse.pop();
                    }

                    return event;
                }
                /** Retrieve a buffered keyboard event.
                */
                static KeyboardEvent* getKeyboardEvent()
                {
                    KeyboardEvent *event = NULL;

                    if (m_BufferKeyboard.size()) {
                        event = m_BufferKeyboard.front();
                        m_BufferKeyboard.pop();
                    }

                    return event;
                }
                /** Retrieve a buffered joystick event.
                */
                static JoystickEvent* getJoystickEvent()
                {
                    JoystickEvent *event = NULL;
                
                    if (m_BufferJoystick.size()) {
                        event = m_BufferJoystick.front();
                        m_BufferJoystick.pop();
                    }

                    return event;
                }

                //
                static Task *pTaskCapture;
                inline static void doTaskCapture_Wrapper(void *_class, void *_args) {
                    reinterpret_cast<InputManager*>(_class)->doTaskCapture(_args);
                };

            protected:
                InputManager();
                InputManager(const InputManager&);
                InputManager& operator=(const InputManager&);
                ~InputManager();

            private:
                void* doTaskCapture(void *_args);

                // Hold a buffer of captured events
                static queue<MouseEvent*>         m_BufferMouse;
                static queue<KeyboardEvent*>      m_BufferKeyboard;
                static queue<JoystickEvent*>      m_BufferJoystick;

                // OIS specific members
                static OIS::Keyboard        *m_pKeyboard;
                static OIS::Mouse           *m_pMouse;
                static OIS::InputManager    *m_pInputManager;
                static vector<JoyStickInputHandler*> *m_pJoyStickList;

                // OIS overloaded event functions
                // KeyListener
                bool keyPressed(const OIS::KeyEvent &arg)
                {
                    KeyboardEvent *newKeyboardEvent = new KeyboardEvent(arg);
                    newKeyboardEvent->m_nKeyboardEventID = KEYBOARD_KEY_PRESSED;
                    newKeyboardEvent->m_nValue = arg.key;
                    m_BufferKeyboard.push(newKeyboardEvent);

                    return true;
                }
                bool keyReleased(const OIS::KeyEvent &arg)
                {
                    KeyboardEvent *newKeyboardEvent = new KeyboardEvent(arg);
                    newKeyboardEvent->m_nKeyboardEventID = KEYBOARD_KEY_RELEASED;
                    newKeyboardEvent->m_nValue = arg.key;
                    m_BufferKeyboard.push(newKeyboardEvent);

                    return true;
                }

                // MouseListener
                bool mouseMoved(const OIS::MouseEvent &arg)
                {
                    MouseEvent *newMouseEvent = new MouseEvent(arg);
                    newMouseEvent->m_nMouseEventID = MOUSE_MOVE;
                    newMouseEvent->m_nAxisX = arg.state.X.abs;
                    newMouseEvent->m_nAxisY = arg.state.Y.abs;
                    m_BufferMouse.push(newMouseEvent);

                    return true;
                }
                bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
                {
                    MouseEvent *newMouseEvent = new MouseEvent(arg);
                    newMouseEvent->m_nMouseButtonID = (MouseButtonID)id;
                    newMouseEvent->m_nMouseEventID = MOUSE_BUTTON_PRESSED;
                    m_BufferMouse.push(newMouseEvent);

                    return true;
                }
                bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
                {
                    MouseEvent *newMouseEvent = new MouseEvent(arg);
                    newMouseEvent->m_nMouseButtonID = (MouseButtonID)id;
                    newMouseEvent->m_nMouseEventID = MOUSE_BUTTON_RELEASED;
                    m_BufferMouse.push(newMouseEvent);

                    return true;
                }

                
        };

        /** Class to provide a wrapper for singleton class InputManager
            @remarks
                This is used to keep Python happy with our singleton class.
        */
        class InputManager_PythonWrap {
        public:
            /** Default constructor.
            */
            InputManager_PythonWrap();
    
            /** Return our wrapped task.
            */
            Task *getTaskCapture() const {
                return InputManager::getSingleton().pTaskCapture;
            }
            
            /** Initialize python wrapper.
            */
            void initialize(size_t windowHnd = 0);

            /** Destroy python wrapper.
            */
            void destroy();
    
            /** Get a buffered mouse event python wrapper.
            */
            MouseEvent* getMouseEvent();
            /** Get a buffered keyboard event python wrapper.
            */
            KeyboardEvent* getKeyboardEvent();
            /** Get a buffered joystick event python wrapper.
            */
            JoystickEvent* getJoystickEvent();
        };

    };
};

#endif  // __F_INPUT_SYSTEM_H_

#endif  // S_ENABLE_INPUT
