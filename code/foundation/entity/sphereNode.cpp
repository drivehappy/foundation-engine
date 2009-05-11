#include "sphereNode.h"

using namespace Foundation;
using namespace Foundation::Entities;

SphereNode::SphereNode(const float _nMinRadius, const float _nMaxRadius)
{
    m_nMinRadius = _nMinRadius;
    m_nMaxRadius = m_nMaxRadius;
    m_nRadius = m_nMinRadius;
    m_bRootNode = false;

    m_nMaxBucketSize = 10;
    m_bDataNode = false;
    m_pData = NULL;

    m_sGraphicID = new char[12];
    sprintf(m_sGraphicID, "%p", this);
}

SphereNode::SphereNode(const float _nMinRadius, const float _nMaxRadius, SphereData *_pData)
{
    m_sGraphicID = new char[12];
    sprintf(m_sGraphicID, "%p", this);

    m_nMinRadius = _nMinRadius;
    m_nMaxRadius = _nMaxRadius;
    m_nRadius = m_nMinRadius;
    m_bRootNode = false;

    m_bDataNode = true;
    m_pData = _pData;
}

SphereNode::SphereNode(const float _nMinRadius, const float _nMaxRadius, bool _bDataNode, unsigned int _nMaxBucketSize)
{
    m_nMinRadius = _nMinRadius;
    m_nMaxRadius = _nMaxRadius;
    m_nRadius = m_nMinRadius;
    m_bRootNode = false;

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
    float       nMin[3] = {HUGE_VAL};
    float       nMax[3] = {0.0f};
    gmtl::Vec3f nPosition;
    float       nRadius;
    float       nDistanceSq;
    float       nRadiiSumDistSq, nRadiiSubDistSq;

    // If we're a data node update our node position to our data
    if (m_bDataNode) {
        m_nPosition = m_pData->getPosition();
    } else {
        // Move through our children and update
        for (itr = m_uNodeChildren.begin(); itr != m_uNodeChildren.end(); itr++) {
            (*itr)->update();
        }

        // Move through our updated children vector and determine
        //  if any lie outside our radius
        for (itr = m_uNodeChildren.begin(); itr != m_uNodeChildren.end(); itr++) {
            nDistanceSq = gmtl::lengthSquared<float, 3>((*itr)->getPosition() - m_nPosition);
            nRadiiSumDistSq = pow((*itr)->getRadius() + m_nRadius, 2.0f);
            nRadiiSubDistSq = pow((*itr)->getRadius() - m_nRadius, 2.0f);
            
            if (nDistanceSq > nRadiiSumDistSq) {
                // (*itr) child (including radius) is completely outside of us, move into the parent
            }

            else if (nDistanceSq > m_nRadius) {
                // (*itr) child (center) is completely outside of us, move into the parent
            }

            else if (nDistanceSq > nRadiiSubDistSq) {
                // (*itr) child (center + radius) is somewhat outside of us
            }
        }

        // After we're done culling out bad ones,
        //  Update our position and radius of our node depending upon our children
        for (itr = m_uNodeChildren.begin(); itr != m_uNodeChildren.end(); itr++) {
            nPosition = (*itr)->getPosition();
            nRadius = (*itr)->getRadius();

            for (int d = 0; d < 3; d++) {
                if (nPosition[d] - nRadius < nMin[d]) {
                    nMin[d] = nPosition[d] - nRadius;
                }
                
                if (nPosition[d] + nRadius > nMax[d]) {
                    nMax[d] = nPosition[d] + nRadius;
                }
            }
        }

        if (m_uNodeChildren.size() == 0) {
            m_nPosition = gmtl::Vec3f(0, 0, 0);
        } else {
            gmtl::Vec2f nRadiusDistance = gmtl::Vec2f(
                nMax[gmtl::Xelt] - nMin[gmtl::Xelt],
                nMax[gmtl::Zelt] - nMin[gmtl::Zelt]);
            m_nRadius = gmtl::length(nRadiusDistance) / 2.0f;

            m_nPosition[gmtl::Xelt] = (nMax[gmtl::Xelt] + nMin[gmtl::Xelt]) / 2.0f;
            m_nPosition[gmtl::Zelt] = (nMax[gmtl::Zelt] + nMin[gmtl::Zelt]) / 2.0f;
        }
    }

    if (!m_bRootNode) {
        if (m_nRadius > m_nMaxRadius) {
            m_nRadius = m_nMaxRadius;
        } else if (m_nRadius < m_nMinRadius) {
            m_nRadius = m_nMinRadius;
        }
    }
}

void SphereNode::addSphereData(SphereData *_data)
{
    //SphereNode *pBestNode = getBestFitNode(_data);
    SphereNode *pNewNode;
    
    gmtl::Vec3f nPosition;
    float       nRadius;

    f_printf(" SphereNode Adding Data\n");

    // Greedy search the child nodes
    //while ( (pBestNode = pBestNode->getBestFitNode(_data)) );

    //f_printf(" SphereNode Found Best Fit Node: %p\n", pBestNode);

    // Test me, this is root and just add all nodes to me
    pNewNode = new SphereNode(m_nMinRadius, m_nMaxRadius, _data);
    m_uNodeChildren.push_back(pNewNode);

    /*
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
    */

    /*
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
    */
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

    // Do Render Circle
    Graphic::GraphicManager::getSingleton().updateCircle(_sSceneManagerName, m_sGraphicID, nPosition, m_nRadius, gmtl::Yelt);

    if (_bRecursive) {
        for (vector<SphereNode *>::iterator itr = m_uNodeChildren.begin(); itr != m_uNodeChildren.end(); itr++) {
            (*itr)->debugRender(_sSceneManagerName, _bRecursive);
        }
    }
}

gmtl::Vec3f SphereNode::getPosition()
{
    return m_nPosition;
}

void SphereNode::setPosition(gmtl::Vec3f _nPosition)
{
    m_nPosition = _nPosition;
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
