#include "sphereNode.h"

using namespace Foundation;
using namespace Foundation::Entity;

SphereNode::SphereNode()
{
}

SphereNode::~SphereNode()
{
}



void SphereNode::debugRender()
{
    // Do Render
}

gmtl::Vec3f SphereNode::getPosition()
{
    return m_nPosition;
}

unsigned int SphereNode::getChildCount()
{
    vector<SphereNode *>::iterator itr;
    unsigned int nCount = 0;

    for (itr = m_uChildren.begin(); itr != m_uChildren.end(); itr++) {
        nCount += (*itr)->getChildCount();
    }

    return nCount;
}

unsigned int SphereNode::getMaxDepth()
{
    vector<SphereNode *>::iterator itr;
    unsigned int nMaxDepth = 0, nTemp;

    for (itr = m_uChildren.begin(); itr != m_uChildren.end(); itr++) {
        nTemp = (*itr)->getMaxDepth();

        if (nTemp > nMaxDepth)
            nMaxDepth = nTemp;
    }

    return nMaxDepth;
}

// TODO: Needs work, find the real average with radii included
void SphereNode::updateToFitChildren(gmtl::Vec3f & _nPosition, float & _nRadius)
{
    vector<SphereNode*>::iterator itr;
    float nDistSq = 0.0f;
    gmtl::Vec3f nTempData;

    _nPosition = gmtl::Vec3f(0, 0, 0);
    for (itr = m_uChildren.begin(); itr != m_uChildren.end(); itr++) {
        nTempData = (*itr)->getPosition();

        _nPosition += nTempData;

        if (gmtl::length<float, 3>(nTempData - m_nPosition) > nDistSq)
            nDistSq = gmtl::length<float, 3>(nTempData - m_nPosition);
    }
    _nPosition /= m_uChildren.size();

    // For now, just test it - will be broken
    _nRadius = nDistSq;
}
