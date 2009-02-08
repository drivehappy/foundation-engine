#include "py_guiSystem.h"

// S_ENABLE_GRAPHIC is defined in premake.lua, it should only be altered there
// to allow for easy stripping/adding of libraries actually needed to be compiled
#if defined(S_ENABLE_GRAPHIC)

using namespace Foundation;
using namespace Foundation::Graphic;


GUIManager_PythonWrapper::GUIManager_PythonWrapper()
{
}

GUIManager_PythonWrapper::~GUIManager_PythonWrapper()
{
}

void GUIManager_PythonWrapper::initialize(RenderWindow_PythonWrapper *_pRenderWindow)
{
    GUIManager::getSingleton().initialize((RenderWindow *)_pRenderWindow);
}

void GUIManager_PythonWrapper::destroy()
{
    GUIManager::getSingleton().destroy();
}

bool GUIManager_PythonWrapper::injectMouseEvent(const Input::MouseEvent & _mEvent)
{
    return GUIManager::getSingleton().injectMouseEvent(_mEvent);
}

bool GUIManager_PythonWrapper::injectKeyboardEvent(const Input::KeyboardEvent & _mEvent)
{
    return GUIManager::getSingleton().injectKeyboardEvent(_mEvent);
}

void GUIManager_PythonWrapper::setMousePosition(unsigned int _nX, unsigned int _nY, unsigned int _nZ)
{
    return GUIManager::getSingleton().setMousePosition(_nX, _nY, _nZ);
}

void GUIManager_PythonWrapper::createWindow(const char *_sParent, const char *_sName, unsigned int _nPositionX, unsigned int _nPositionY, unsigned int _nSizeX, unsigned int _nSizeY, const char *_sCaption)
{
    GUIManager::getSingleton().createWindow(_sParent, _sName, _nPositionX, _nPositionY, _nSizeX, _nSizeY, _sCaption);
}

void GUIManager_PythonWrapper::createButton(const char *_sParent, const char *_sName, unsigned int _nPositionX, unsigned int _nPositionY, unsigned int _nSizeX, unsigned int _nSizeY, const char *_sCaption)
{
    GUIManager::getSingleton().createButton(_sParent, _sName, _nPositionX, _nPositionY, _nSizeX, _nSizeY, _sCaption);
}

void GUIManager_PythonWrapper::createStaticText(const char *_sParent, const char *_sName, unsigned int _nPositionX, unsigned int _nPositionY, unsigned int _nSizeX, unsigned int _nSizeY, const char *_sCaption)
{
    GUIManager::getSingleton().createStaticText(_sParent, _sName, _nPositionX, _nPositionY, _nSizeX, _nSizeY, _sCaption);
}

void GUIManager_PythonWrapper::setWidgetCaption(const char *_sName, const char *_sCaption)
{
    GUIManager::getSingleton().setWidgetCaption(_sName, _sCaption);
}

void GUIManager_PythonWrapper::setObjectColor(const char *_sObjectName, float _nR, float _nG, float _nB)
{
    GUIManager::getSingleton().setObjectColor(_sObjectName, _nR, _nG, _nB);
}

void GUIManager_PythonWrapper::setTextFont(const char *_sObjectName, const char *_sFontFace, int _nFontSize)
{
    GUIManager::getSingleton().setTextFont(_sObjectName, _sFontFace, _nFontSize);
}

void GUIManager_PythonWrapper::destroyObject(const char *_sName)
{
    GUIManager::getSingleton().destroyObject(_sName);
}

void GUIManager_PythonWrapper::setActiveViewport(const unsigned short _nViewport)
{
    GUIManager::getSingleton().setActiveViewport(_nViewport);
}

void GUIManager_PythonWrapper::setWidgetVisible(const char *_sName, const bool _bVisible)
{
    GUIManager::getSingleton().setWidgetVisible(_sName, _bVisible);
}

#endif  // S_ENABLE_GRAPHIC
