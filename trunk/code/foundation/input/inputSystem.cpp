#include "inputSystem.h"

// S_ENABLE_INPUT is defined in premake.lua, it should only be altered there
// to allow for easy stripping/adding of libraries actually needed to be compiled
#if defined(S_ENABLE_INPUT)

using namespace Foundation::Input;

// Member variables
OIS::Keyboard           *InputManager::m_pKeyboard    = NULL;
OIS::Mouse              *InputManager::m_pMouse       = NULL;
vector<JoyStickInputHandler*> *InputManager::m_pJoyStickList = NULL;
OIS::InputManager       *InputManager::m_pInputManager= NULL;
Task                    *InputManager::pTaskCapture   = NULL;
queue<MouseEvent*>      InputManager::m_BufferMouse;
queue<KeyboardEvent*>   InputManager::m_BufferKeyboard;
queue<JoystickEvent*>   InputManager::m_BufferJoystick;

InputManager& InputManager::getSingleton()
{
    static InputManager m_Singleton;
    return m_Singleton;
}

InputManager::InputManager()
{
    pTaskCapture = new Task(this, InputManager::doTaskCapture_Wrapper);
    //pTaskCapture->_this = this;
    //pTaskCapture->_functionPointer = InputManager::doTaskCapture_Wrapper;

    m_pInputManager = NULL;
}

InputManager::~InputManager()
{
    destroy();
}


void InputManager::initialize(size_t windowHnd)
{
    std::ostringstream windowHndStr;
    OIS::ParamList pl;

    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
#if FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_WIN
    pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
    pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
#elif FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_LINUX
    pl.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
    pl.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("true")));
#endif
    m_pInputManager = OIS::InputManager::createInputSystem(pl);

    m_pJoyStickList = new vector<JoyStickInputHandler*>();

    try {
        m_pKeyboard = static_cast<OIS::Keyboard*>(m_pInputManager->createInputObject(OIS::OISKeyboard, true));
    } catch (const OIS::Exception &e) {
        f_printf(" - [InputManager] Warning (Keyboard): %s\n", e.eText);
    }
    try {
        m_pMouse = static_cast<OIS::Mouse*>(m_pInputManager->createInputObject(OIS::OISMouse, true));
    } catch (const OIS::Exception &e) {
        f_printf(" - [InputManager] Warning (Mouse): %s\n", e.eText);
    }

    OIS::JoyStick *pJoyStick0;
    OIS::JoyStick *pJoyStick1;
    try {
        pJoyStick0 = static_cast<OIS::JoyStick*>(m_pInputManager->createInputObject(OIS::OISJoyStick, true));
        InputManager::getSingleton().setupJoystickDevice(pJoyStick0);
    } catch (const OIS::Exception &e) {
        f_printf(" - [InputManager] Warning (Joystick0): %s\n", e.eText);
    }
    try {
        pJoyStick1 = static_cast<OIS::JoyStick*>(m_pInputManager->createInputObject(OIS::OISJoyStick, true));
        InputManager::getSingleton().setupJoystickDevice(pJoyStick1);
    } catch (const OIS::Exception &e) {
        f_printf(" - [InputManager] Warning (Joystick1): %s\n", e.eText);
    }

    InputManager::getSingleton().setupInputDevices(m_pKeyboard, m_pMouse);
}

void InputManager::destroy()
{
    if (m_pInputManager) {
        m_pInputManager->destroyInputObject(m_pKeyboard);
        m_pInputManager->destroyInputObject(m_pMouse);
        //m_pInputManager->destroyInputObject(m_pJoyStick);
        //m_pInputManager->destroyInputObject(m_pJoyStick1);
    
        OIS::InputManager::destroyInputSystem(m_pInputManager);
        m_pInputManager = NULL;
    }
}

void InputManager::setWindowExtents(unsigned int _nWidth, unsigned int _nHeight)
{
    const OIS::MouseState& mMouseState = m_pMouse->getMouseState();

    mMouseState.width = _nWidth;
    mMouseState.height = _nHeight;

    f_printf(" - [InputManager] Set Window Extents to %i, %i\n", _nWidth, _nHeight);
}

void* InputManager::doTaskCapture(void *_args)
{
    if (m_pKeyboard)
        m_pKeyboard->capture();

    if (m_pMouse)
        m_pMouse->capture();

    vector<JoyStickInputHandler*>::iterator itrVec;
    for (itrVec = m_pJoyStickList->begin(); itrVec != m_pJoyStickList->end(); itrVec++) {
        if ((*itrVec)) {
            (*itrVec)->capture();            
        }

        queue<JoystickEvent *> eventList = (*itrVec)->getEventList();

        while (eventList.size()) {
            JoystickEvent *event = eventList.front();
            eventList.pop();
            
            m_BufferJoystick.push(event);
        }
    }

    return NULL;
}


InputManager_PythonWrap::InputManager_PythonWrap()
{
}

void InputManager_PythonWrap::initialize(size_t windowHnd)
{
    return InputManager::getSingleton().initialize(windowHnd);
}

void InputManager_PythonWrap::destroy()
{
    return InputManager::getSingleton().destroy();
}

MouseEvent* InputManager_PythonWrap::getMouseEvent()
{
    return InputManager::getSingleton().getMouseEvent();
}

KeyboardEvent* InputManager_PythonWrap::getKeyboardEvent()
{
    return InputManager::getSingleton().getKeyboardEvent();
}

JoystickEvent* InputManager_PythonWrap::getJoystickEvent()
{
    return InputManager::getSingleton().getJoystickEvent();
}

#endif  // S_ENABLE_INPUT
