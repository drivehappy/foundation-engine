#include "sphereNode.h"

using namespace Foundation;
using namespace Foundation::Entities;

SphereNode::SphereNode()
{
    m_nMaxBucketSize = 10;
    m_bDataNode = false;
    m_pData = NULL;
}

SphereNode::SphereNode(bool _bDataNode, unsigned int _nMaxBucketSize)
{
    m_nMaxBucketSize = _nMaxBucketSize;
    m_bDataNode = _bDataNode;
    m_pData = NULL;
}

SphereNode::~SphereNode()
{
}


void SphereNode::update()
{
    vector<SphereNode *>::iterator itr;

    float nDistanceSq;
    float nRadiiSumDistSq, nRadiiSubDistSq;

    // Move through our children vector and determine if any lie outside our radius
    for (itr = m_uNodeChildren.begin(); itr != m_uNodeChildren.end(); itr++) {
        nDistanceSq = gmtl::lengthSquared<float, 3>((*itr)->getPosition() - m_nPosition);
        nRadiiSumDistSq = pow((*itr)->getRadius() + m_nRadius, 2.0f);
        nRadiiSubDistSq = pow((*itr)->getRadius() - m_nRadius, 2.0f);

        if (nDistanceSq > nRadiiSumDistSq) {
            // (*itr) child (including radius) is completely outside of us, move into the parent
        }

        if (nDistanceSq > m_nRadius) {
            // (*itr) child (center) is completely outside of us, move into the parent
        }

        if (nDistanceSq > nRadiiSubDistSq) {
            // (*itr) child (center + radius) is somewhat outside of us
        }
    }
}

SphereNode* SphereNode::getBestFitNode(SphereData* _uItr)
{
    // Determine if this point fits within us
    gmtl::Vec3f nPoint = (*_uItr).getPosition();
    float nRadiusSq = pow(m_nRadius, 2.0f);
    float nDistanceSq;
    SphereNode *bestNode;
    vector<SphereNode *>::iterator itr;

    nDistanceSq = gmtl::lengthSquared<float, 3>(nPoint - m_nPosition);
    
    if (nDistanceSq < nRadiusSq) {
        // Yay it fits, try out the children now, somehow determine which is best
        //  perhaps recurse through it find any non null nodes and check against each?
        for (itr = m_uNodeChildren.begin(); itr != m_uNodeChildren.end(); itr++) {
            
        }

        //bestNode = ??
    } else {
        // Nay it doesn't
        bestNode = NULL;
    }

    return bestNode;
}

void SphereNode::debugRender()
{
    // Do Render Circle
}

gmtl::Vec3f SphereNode::getPosition()
{
    return m_nPosition;
}

float SphereNode::getRadius() const
{
    return m_nRadius;
}

unsigned int SphereNode::getChildCount()
{
    vector<SphereNode *>::iterator itr;
    unsigned int nCount = 0;

    for (itr = m_uNodeChildren.begin(); itr != m_uNodeChildren.end(); itr++) {
        nCount += (*itr)->getChildCount();
    }

    return nCount;
}

unsigned int SphereNode::getMaxDepth()
{
    vector<SphereNode *>::iterator itr;
    unsigned int nMaxDepth = 0, nTemp;

    for (itr = m_uNodeChildren.begin(); itr != m_uNodeChildren.end(); itr++) {
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
    for (itr = m_uNodeChildren.begin(); itr != m_uNodeChildren.end(); itr++) {
        nTempData = (*itr)->getPosition();

        _nPosition += nTempData;

        if (gmtl::length<float, 3>(nTempData - m_nPosition) > nDistSq)
            nDistSq = gmtl::length<float, 3>(nTempData - m_nPosition);
    }
    _nPosition /= m_uNodeChildren.size();

    // For now, just test it - will be broken
    _nRadius = nDistSq;
}
