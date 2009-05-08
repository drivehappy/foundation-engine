#include "sphereNode.h"

using namespace Foundation;
using namespace Foundation::Entities;

SphereNode::SphereNode()
{
    m_nMaxBucketSize = 10;
    m_bDataNode = false;
    m_pData = NULL;

    m_sGraphicID = new char[12];
    sprintf(m_sGraphicID, "%p", this);
}

SphereNode::SphereNode(const SphereData & _pData)
{
    m_pData = new SphereData(_pData);
}

SphereNode::SphereNode(bool _bDataNode, unsigned int _nMaxBucketSize)
{
    m_nMaxBucketSize = _nMaxBucketSize;
    m_bDataNode = _bDataNode;
    m_pData = NULL;

    m_sGraphicID = new char[12];
    sprintf(m_sGraphicID, "%p", this);
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

void SphereNode::addSphereData(const SphereData & _data)
{
    SphereNode *pBestNode = getBestFitNode(_data);
    SphereNode *pNewNode;
    float       nMin[3] = {HUGE_VAL};
    float       nMax[3] = {0.0f};
    gmtl::Vec3f nPosition;
    float       nRadius;

    f_printf(" SphereNode Adding Data\n");

    // Greedy search the child nodes
    //while ( (pBestNode = pBestNode->getBestFitNode(_data)) );

    f_printf(" SphereNode Found Best Fit Node: %p\n", pBestNode);

    // Take the node if we're the best parent for it
    if (pBestNode == this) {
        pNewNode = new SphereNode(_data);
        m_uNodeChildren.push_back(pNewNode);

        // Find our new center, this is based on the min/max of each dimension, radius' of our children
        for (vector<SphereNode *>::iterator itr = m_uNodeChildren.begin(); itr != m_uNodeChildren.end(); itr++) {
            nPosition = (*itr)->getPosition();
            nRadius = (*itr)->getRadius();

            for (int d = 0; d < 3; d++) {
                if (nPosition[d] - nRadius < nMin[d]) {
                    nMin[d] = nPosition[d] - nRadius;
                } else if (nPosition[d] + nRadius > nMax[d]) {
                    nMax[d] = nPosition[d] + nRadius;
                }
            }
        }

        // Adjust our radius if needed
        
    }
}

SphereNode* SphereNode::getBestFitNode(const SphereData & _uData)
{
    // Determine if this point fits within us
    gmtl::Vec3f nPoint = _uData.getPosition();
    float nRadiusSq = pow(m_nRadius, 2.0f);
    float nDistanceSq;
    SphereNode *bestNode = NULL;
    vector<SphereNode *>::iterator itr;

    nDistanceSq = gmtl::lengthSquared<float, 3>(nPoint - m_nPosition);
    
    f_printf("  Getting best fit node: %f < %f\n", nDistanceSq, nRadiusSq);

    if (nDistanceSq < nRadiusSq) {
        // It fits, try out the children now, somehow determine which is best
        //  perhaps recurse through it find any non null nodes and check against each?
        for (itr = m_uNodeChildren.begin(); itr != m_uNodeChildren.end(); itr++) {
            f_printf("    Child %p\n", (*itr));
            bestNode = (*itr)->getBestFitNode(_uData);

            if (bestNode)
                break;
        }

        if (!bestNode)
            bestNode = this;
    } else {
        bestNode = this;
    }

    return bestNode;
}

void SphereNode::debugRender(const char* _sSceneManagerName, bool _bRecursive)
{
    gmtl::Vec3f nPosition = m_nPosition;
    
    nPosition[gmtl::Yelt] = 15.0f;

    f_printf("Rendering my Debug Circle: %f, %f, %f   %f\n", m_nPosition[gmtl::Xelt], m_nPosition[gmtl::Yelt], m_nPosition[gmtl::Zelt], m_nRadius);

    // Do Render Circle
    Graphic::GraphicManager::getSingleton().updateCircle(_sSceneManagerName, m_sGraphicID, nPosition, m_nRadius, gmtl::Yelt);

    if (_bRecursive) {
        for (vector<SphereNode *>::iterator itr = m_uNodeChildren.begin(); itr != m_uNodeChildren.end(); itr++) {
            (*itr)->debugRender(_sSceneManagerName, _bRecursive);
        }
    }

    f_printf("/Done Rendering\n");
}

gmtl::Vec3f SphereNode::getPosition()
{
    return m_nPosition;
}

float SphereNode::getRadius() const
{
    return m_nRadius;
}

void SphereNode::setRadius(float _nRadius)
{
    m_nRadius = _nRadius;
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
