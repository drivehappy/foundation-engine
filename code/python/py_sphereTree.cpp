#include "py_sphereTree.h"

using namespace Foundation;
using namespace Foundation::Entities;

// SphereDataIter
SphereDataIterator_PythonWrapper::SphereDataIterator_PythonWrapper()
{
}

// SphereData
gmtlVector3_PythonWrapper SphereData_PythonWrapper::getPosition()
{
    return (gmtlVector3_PythonWrapper)SphereData::getPosition();
}

void SphereData_PythonWrapper::setPosition(const gmtlVector3_PythonWrapper _nPosition)
{
    SphereData::setPosition(_nPosition);
}

// SphereTree
SphereTree_PythonWrapper::SphereTree_PythonWrapper()
{
}

void SphereTree_PythonWrapper::addData(SphereData_PythonWrapper *_uData)
{
    SphereTree::addData(_uData);
}
void SphereTree_PythonWrapper::debugRender(const char* _sSceneManagerName, int _nRenderLevel, int _nTeamBitfieldFlag)
{
    SphereTree::debugRender(_sSceneManagerName, _nRenderLevel, _nTeamBitfieldFlag);
}

