#include "guiSystem.h"

// S_ENABLE_GRAPHIC is defined in premake.lua, it should only be altered there
// to allow for easy stripping/adding of libraries actually needed to be compiled
#if defined(S_ENABLE_GRAPHIC)

using namespace Foundation;
using namespace Foundation::Graphic;


// Static data members
MyGUI::Gui          *GUIManager::m_pGUI;
Channel             *GUIManager::m_pMouseEventChannel;

// GUIManager
GUIManager& GUIManager::getSingleton()
{
    static GUIManager mSingleton;

    return mSingleton;
}

GUIManager::GUIManager()
{
    m_pGUI = NULL;
    m_pMouseEventChannel = new Channel();
    m_pMouseEventChannel->Channel_Join("MOUSE_EVENTS", NULL);
}

GUIManager::~GUIManager()
{
    destroy();
}


void GUIManager::initialize(RenderWindow *_pRenderWindow)
{
    if (_pRenderWindow) {
        if (_pRenderWindow->getNumViewports() > 0) {
            m_pGUI = new MyGUI::Gui();
            m_pGUI->initialise(_pRenderWindow);

            Input::InputManager::getSingleton().setWindowExtents(_pRenderWindow->getWidth(), _pRenderWindow->getHeight());
        } else {
            f_printf(" - [GUIManager] Error: RenderTarget has 0 viewports.\n");
        }
    } else {
        f_printf(" - [GUIManager] Error: RenderWindow is NULL.\n");
    }
}

void GUIManager::destroy()
{
    //m_pGUI->shutdown();
    m_pGUI = NULL;

    if (m_pMouseEventChannel)
        m_pMouseEventChannel->Channel_Leave("MOUSE_EVENTS", NULL);
}

bool GUIManager::injectMouseEvent(const Input::MouseEvent & _mEvent)
{
    switch (_mEvent.m_nMouseEventID) {
        case Input::MOUSE_MOVE:
            return m_pGUI->injectMouseMove(_mEvent);
            break;
        case Input::MOUSE_BUTTON_PRESSED:
            return m_pGUI->injectMousePress(_mEvent, (OIS::MouseButtonID)_mEvent.m_nMouseButtonID);
            break;
        case Input::MOUSE_BUTTON_RELEASED:
            return m_pGUI->injectMouseRelease(_mEvent, (OIS::MouseButtonID)_mEvent.m_nMouseButtonID);
            break;
        default:
            break;
    }

    return false;
}

bool GUIManager::injectKeyboardEvent(const Input::KeyboardEvent & _mEvent)
{
    switch (_mEvent.m_nKeyboardEventID) {
        case Input::KEYBOARD_KEY_PRESSED:
            return m_pGUI->injectKeyPress(_mEvent);
            break;
        case Input::KEYBOARD_KEY_RELEASED:
            return m_pGUI->injectKeyRelease(_mEvent);
            break;
        default:
            break;
    }

    return false;
}

void GUIManager::eventMouseButtonPressed(MyGUI::WidgetPtr _pSender, int _nX, int _nY, MyGUI::MouseButton _nId)
{
    char *pData, nData;
    Ogre::String sObjectName = _pSender->getName();
    unsigned int nDataSize = (unsigned int)sObjectName.length();

    pData = new char[nDataSize];
    nData = Graphic::PRESSED;

    if (m_pMouseEventChannel) {
        memcpy(pData, sObjectName.c_str(), sObjectName.length());
        m_pMouseEventChannel->Channel_MessageCreate(nDataSize);
        m_pMouseEventChannel->Channel_MessageHeader(nData);
        m_pMouseEventChannel->Channel_MessageData(pData, nDataSize);
        m_pMouseEventChannel->Channel_Send("MOUSE_EVENTS");
        m_pMouseEventChannel->Channel_MessageDestroy();
    }
}

void GUIManager::eventMouseButtonReleased(MyGUI::WidgetPtr _pSender, int _nX, int _nY, MyGUI::MouseButton _nId)
{
    char *pData, nData;
    Ogre::String sObjectName = _pSender->getName();
    unsigned int nDataSize = (unsigned int)sObjectName.length();

    pData = new char[nDataSize];
    nData = Graphic::RELEASED;

    if (m_pMouseEventChannel) {
        memcpy(pData, sObjectName.c_str(), sObjectName.length());
        m_pMouseEventChannel->Channel_MessageCreate(nDataSize);
        m_pMouseEventChannel->Channel_MessageHeader(nData);
        m_pMouseEventChannel->Channel_MessageData(pData, nDataSize);
        m_pMouseEventChannel->Channel_Send("MOUSE_EVENTS");
        m_pMouseEventChannel->Channel_MessageDestroy();
    }
}

void GUIManager::eventMouseMove(MyGUI::WidgetPtr _pSender, int _nX, int _nY)
{
    char *pData, nData;
    Ogre::String sObjectName = _pSender->getName();
    unsigned int nDataSize = (unsigned int)sObjectName.length();

    pData = new char[nDataSize];
    nData = Graphic::MOVE;

    if (m_pMouseEventChannel) {
        memcpy(pData, sObjectName.c_str(), sObjectName.length());
        m_pMouseEventChannel->Channel_MessageCreate(nDataSize);
        m_pMouseEventChannel->Channel_MessageHeader(nData);
        m_pMouseEventChannel->Channel_MessageData(pData, nDataSize);
        m_pMouseEventChannel->Channel_Send("MOUSE_EVENTS");
        m_pMouseEventChannel->Channel_MessageDestroy();
    }
}

void GUIManager::setMousePosition(unsigned int _nX, unsigned int _nY, unsigned int _nZ)
{
    m_pGUI->injectMouseMove(_nX, _nY, _nZ);
}

void GUIManager::createWindow(const char *_sParent, const char *_sName, unsigned int _nPositionX, unsigned int _nPositionY, unsigned int _nSizeX, unsigned int _nSizeY, const char *_sCaption)
{
    MyGUI::WindowPtr pWindow;
    MyGUI::WidgetPtr pParent, pWidget;
    std::string sParent(_sParent);
    std::string sName(_sName);

    pParent = m_pGUI->findWidgetT(sParent, false);
    pWidget = m_pGUI->findWidgetT(sName, false);
    if (!pWidget) {
        if (!pParent)
            pWindow = m_pGUI->createWidget<MyGUI::Window>("WindowCSX", _nPositionX, _nPositionY, _nSizeX, _nSizeY, MyGUI::ALIGN_LEFT | MyGUI::ALIGN_TOP, "Overlapped", _sName);
        else
            pWindow = pParent->createWidget<MyGUI::Window>("WindowCSX", _nPositionX, _nPositionY, _nSizeX, _nSizeY, MyGUI::ALIGN_LEFT | MyGUI::ALIGN_TOP, _sName);

        pWindow->setCaption(_sCaption);
        //pWindow->setAutoAlpha(false);
        //pWindow->setMinMax(MyGUI::IntRect(_nPositionX, _nPositionY, _nSizeX, _nSizeY));
        //pWindow->showSmooth(true);
        pWindow->setVisible(true);
    }
}

void GUIManager::createButton(const char *_sParent, const char *_sName, unsigned int _nPositionX, unsigned int _nPositionY, unsigned int _nSizeX, unsigned int _nSizeY, const char *_sCaption)
{
    MyGUI::ButtonPtr pButton;
    MyGUI::WidgetPtr pParent;
    std::string sParent(_sParent);

    pParent = m_pGUI->findWidgetT(sParent);
    if (!pParent)
        pButton = m_pGUI->createWidget<MyGUI::Button>("Button", _nPositionX, _nPositionY, _nSizeX, _nSizeY, MyGUI::ALIGN_DEFAULT, "Overlapped", _sName);
    else
        pButton = pParent->createWidget<MyGUI::Button>("Button", _nPositionX, _nPositionY, _nSizeX, _nSizeY, MyGUI::ALIGN_DEFAULT, _sName);
    
    pButton->eventMouseButtonPressed = MyGUI::newDelegate(eventMouseButtonPressed);
    pButton->eventMouseButtonReleased = MyGUI::newDelegate(eventMouseButtonReleased);
    pButton->eventMouseMove = MyGUI::newDelegate(eventMouseMove);

    pButton->setCaption(_sCaption);
}

void GUIManager::createStaticText(const char *_sParent, const char *_sName, unsigned int _nPositionX, unsigned int _nPositionY, unsigned int _nSizeX, unsigned int _nSizeY, const char *_sCaption)
{
    MyGUI::StaticTextPtr pStaticText;
    MyGUI::WidgetPtr pParent, pText;
    std::string sParent(_sParent);

    pParent = m_pGUI->findWidgetT(sParent, false);
    pText = m_pGUI->findWidgetT(_sName, false);
    if (!pText) {
        if (!pParent)
            pStaticText = m_pGUI->createWidget<MyGUI::StaticText>("StaticText", _nPositionX, _nPositionY, _nSizeX, _nSizeY, MyGUI::ALIGN_DEFAULT, "Overlapped", _sName);
        else
            pStaticText = pParent->createWidget<MyGUI::StaticText>("StaticText", _nPositionX, _nPositionY, _nSizeX, _nSizeY, MyGUI::ALIGN_DEFAULT, _sName);

        pStaticText->setCaption(_sCaption);
    }
}

void GUIManager::setObjectColor(const char *_sObjectName, float _nR, float _nG, float _nB)
{
    MyGUI::WidgetPtr pWidget;

    pWidget = m_pGUI->findWidgetT(_sObjectName, false);
    if (pWidget) {
        pWidget->setTextColour(MyGUI::Colour(_nR, _nG, _nB, 1.0));
    }
}

void GUIManager::setTextFont(const char *_sObjectName, const char *_sFontFace, int _nFontSize)
{
    MyGUI::WidgetPtr pWidget;

    pWidget = m_pGUI->findWidgetT(_sObjectName, false);
    if (pWidget) {
        pWidget->setFontName(std::string(_sFontFace));
        pWidget->setFontHeight((MyGUI::uint)_nFontSize);
    }
}

void GUIManager::destroyObject(const char *_sName)
{
    MyGUI::WidgetPtr pWidget;

    pWidget = m_pGUI->findWidgetT(_sName, false);
    if (pWidget)
        m_pGUI->destroyWidget(pWidget);
}

void GUIManager::setWidgetCaption(const char *_sName, const char *_sCaption)
{
    MyGUI::WidgetPtr pWidget;

    pWidget = m_pGUI->findWidgetT(_sName, false);
    if (pWidget)
        pWidget->setCaption(_sCaption);
}

void GUIManager::setActiveViewport(const unsigned short _nViewport)
{
    m_pGUI->setActiveViewport((Ogre::ushort)_nViewport);
}

void GUIManager::setWidgetVisible(const char *_sName, const bool _bVisible)
{
    MyGUI::WidgetPtr pWidget;

    pWidget = m_pGUI->findWidgetT(_sName, false);
    if (pWidget)
        pWidget->setVisible(_bVisible);
}


#endif  // S_ENABLE_GRAPHIC
