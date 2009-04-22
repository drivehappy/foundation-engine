#include "py_graphicManager.h"

// S_ENABLE_GRAPHIC is defined in premake.lua, it should only be altered there
// to allow for easy stripping/adding of libraries actually needed to be compiled
#if defined(S_ENABLE_GRAPHIC)

using namespace Foundation;
using namespace Foundation::Graphic;

// Camera
Camera_PythonWrapper::Camera_PythonWrapper(const char *_sName, SceneManager *_pSceneManager)
: Camera(_sName, _pSceneManager)
{
    // do nothing
}

gmtlVector3_PythonWrapper Camera_PythonWrapper::getPosition() const
{
    return (gmtlVector3_PythonWrapper)Camera::getPosition();
}

gmtlQuaternion_PythonWrapper Camera_PythonWrapper::getRotation() const
{
    return (gmtlQuaternion_PythonWrapper)Camera::getRotation();
}

bool Camera_PythonWrapper::isObjectVisible(const gmtlVector3_PythonWrapper _nPosition) const
{
    return Camera::isObjectVisible(gmtl::Vec3f(_nPosition));
}

/*
void Camera_PythonWrapper::setAspectRatio(const float _nRatio)
{
    Camera::setAspectRatio(_nRatio);
}
*/

// RenderWindow
unsigned int RenderWindow_PythonWrapper::getWidth() const
{
    return RenderWindow::getWidth();
}

unsigned int RenderWindow_PythonWrapper::getHeight() const
{
    return RenderWindow::getHeight();
}

// GraphicManager
GraphicManager_PythonWrapper::GraphicManager_PythonWrapper()
{
}

GraphicManager_PythonWrapper::~GraphicManager_PythonWrapper()
{
}

void GraphicManager_PythonWrapper::showCursor(const bool _bShowCursor)
{
    GraphicManager::getSingleton().showCursor(_bShowCursor);
}

void GraphicManager_PythonWrapper::createLight(const char *_sSceneManagerName, const char *_sLightName, const gmtl::Vec3f _nPosition, const gmtl::Vec3f _nDiffuse, const gmtl::Vec3f _nSpecular)
{
    GraphicManager::getSingleton().createLight(_sSceneManagerName, _sLightName, _nPosition, _nDiffuse, _nSpecular);
}

void GraphicManager_PythonWrapper::clearBloomCompositors()
{
    GraphicManager::getSingleton().clearBloomCompositors();
}

void GraphicManager_PythonWrapper::enableBloom(const bool _bEnabled)
{
    GraphicManager::getSingleton().enableBloom(_bEnabled);
}

void GraphicManager_PythonWrapper::setShadowTechnique(const char *_sSceneManagerName)
{
    GraphicManager::getSingleton().setShadowTechnique(_sSceneManagerName);
}

Task* GraphicManager_PythonWrapper::getTaskRender() const
{
    return GraphicManager::getSingleton().getTask();
}

bool GraphicManager_PythonWrapper::initialize(const char *_sWindowTitle)
{
    return GraphicManager::getSingleton().initialize(_sWindowTitle);
}

void GraphicManager_PythonWrapper::destroy()
{
    GraphicManager::getSingleton().destroy();
}

Camera_PythonWrapper* GraphicManager_PythonWrapper::addCamera(const char *_sSceneManagerName, const char *_sCameraName, const int _nZOrder, const float _nViewportLeft, const float _nViewportTop, const float _nViewportWidth, const float _nViewportHeight)
{
    return (Camera_PythonWrapper *)GraphicManager::getSingleton().addCamera(_sSceneManagerName, _sCameraName, _nZOrder, _nViewportLeft, _nViewportTop, _nViewportWidth, _nViewportHeight);
}

Camera_PythonWrapper* GraphicManager_PythonWrapper::getCamera(const char *_sSceneManagerName, const char *_sCameraName) const
{
    return (Camera_PythonWrapper *)GraphicManager::getSingleton().getCamera(_sSceneManagerName, _sCameraName);
}

RenderWindow_PythonWrapper* GraphicManager_PythonWrapper::getRenderWindow() const
{
    return (RenderWindow_PythonWrapper *)GraphicManager::getSingleton().getRenderWindow();
}

void GraphicManager_PythonWrapper::addLine(const char *_sSceneManagerName, const char *_sID, list _nPoints, tuple _nColor)
{
    tuple uTempPoints;
    gmtl::Vec3f nPoint;
    float nPointX, nPointY, nPointZ;
    vector<gmtl::Vec3f> nNewPointList;
    float nRed, nGreen, nBlue;

    // Convert the list of tuples to a vector of points
    for (int x = 0; x < len(_nPoints); x++) {
        uTempPoints = extract<tuple>(_nPoints[x])();
        if (len(uTempPoints) == 3) {
            nPointX = extract<float>(uTempPoints[0])();
            nPointY = extract<float>(uTempPoints[1])();
            nPointZ = extract<float>(uTempPoints[2])();

            nPoint = gmtl::Vec3f(nPointX, nPointY, nPointZ);
        } else {
            f_printf("[GraphicManager] Error: addline requires a list of tuples with 2 sets of points. (%i given)\n", len(uTempPoints));
            return;
        }
        nNewPointList.push_back(nPoint);
    }

    // Convert the color tuple to floats
    nRed = extract<float>(_nColor[0])();
    nGreen = extract<float>(_nColor[1])();
    nBlue = extract<float>(_nColor[2])();

    // Finally call the scarab function to add the lines
    GraphicManager::getSingleton().addLine(_sSceneManagerName, _sID, nNewPointList, nRed, nGreen, nBlue);
}

void GraphicManager_PythonWrapper::updateLine(const char *_sSceneManagerName, const char *_sID, list _nPoints)
{
    tuple uTempPoints;
    gmtl::Vec3f nPoint;
    float nPointX, nPointY, nPointZ;
    vector<gmtl::Vec3f> nNewPointList;

    // Convert the list of tuples to a vector of points
    for (int x = 0; x < len(_nPoints); x++) {
        uTempPoints = extract<tuple>(_nPoints[x])();
        if (len(uTempPoints) == 3) {
            nPointX = extract<float>(uTempPoints[0])();
            nPointY = extract<float>(uTempPoints[1])();
            nPointZ = extract<float>(uTempPoints[2])();

            nPoint = gmtl::Vec3f(nPointX, nPointY, nPointZ);
        } else {
            f_printf("[GraphicManager] Error: addline requires a list of tuples with 2 sets of points. (%i given)\n", len(uTempPoints));
            return;
        }
        nNewPointList.push_back(nPoint);
    }

    // Finally call the scarab function to update the lines
    GraphicManager::getSingleton().updateLine(_sSceneManagerName, _sID, nNewPointList);
}

void GraphicManager_PythonWrapper::addCircle(const char *_sSceneManagerName, const char *_sID, gmtlVector3_PythonWrapper _nPosition, float _nRadius, gmtl::VectorIndex _nPlane, float _nRed, float _nGreen, float _nBlue)
{
	GraphicManager::getSingleton().addCircle(_sSceneManagerName, _sID, _nPosition, _nRadius, _nPlane, _nRed, _nGreen, _nBlue);
}

void GraphicManager_PythonWrapper::updateCircle(const char *_sSceneManagerName, const char *_sID, gmtlVector3_PythonWrapper _nPosition, float _nRadius, gmtl::VectorIndex _nPlane)
{
	GraphicManager::getSingleton().updateCircle(_sSceneManagerName, _sID, _nPosition, _nRadius, _nPlane);
}

void GraphicManager_PythonWrapper::doPicking(const char *sSceneManagerName, const char *sCameraName, const unsigned int _nQueryMask, gmtlVector4_PythonWrapper _nWorldRect, gmtlVector2_PythonWrapper _nScreenWidth)
{
    GraphicManager::getSingleton().doPicking(sSceneManagerName, sCameraName, _nQueryMask, _nWorldRect, _nScreenWidth);
}

PyObject* GraphicManager_PythonWrapper::consumeSelectedObjectName()
{
    char *pString = GraphicManager::getSingleton().consumeSelectedObjectName();
    
    if (pString)
        return PyString_FromString(reinterpret_cast<char *>(pString));
    return NULL;
}

gmtlQuaternion_PythonWrapper GraphicManager_PythonWrapper::getMeshRotation(const char *_sSceneManagerName, const char *_sID)
{
    return GraphicManager::getSingleton().getMeshRotation(_sSceneManagerName, _sID);
}

#endif  // S_ENABLE_GRAPHIC
