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
    vector<SphereNode *>::iterator itr, itrTemp;
    float       nMin[3] = {HUGE_VAL};
    float       nMax[3] = {0.0f};
    gmtl::Vec3f nPosition;
    float       nDistanceSq;
    float       nRadiiSumDistSq, nRadiiSubDistSq, nRadiiDistSq;

    // If we're a data node update our node position to our data
    if (m_bDataNode) {
        m_nPosition = m_pData->getPosition();
    } else {
        // Move through our children and update
        for (int i = 0; i < m_uNodeChildren.size(); i++) {
        //for (itr = m_uNodeChildren.begin(); itr != m_uNodeChildren.end(); itr++) {
        //    (*itr)->update();
            m_uNodeChildren[i]->update();
        }

        if (!m_bRootNode) {
            // Move through our updated children vector and determine
            //  if any lie outside our radius
            for (itr = m_uNodeChildren.begin(); itr != m_uNodeChildren.end(); /* Do nothing */) {
                nDistanceSq = gmtl::lengthSquared<float, 3>((*itr)->getPosition() - m_nPosition);
                nRadiiSumDistSq = pow((*itr)->getRadius() + m_nRadius, 2.0f);
                nRadiiSubDistSq = pow((*itr)->getRadius() - m_nRadius, 2.0f);
                nRadiiDistSq = pow(m_nRadius, 2.0f);
                
                if ((nDistanceSq > nRadiiSumDistSq) ||
                    (nDistanceSq > nRadiiDistSq) ||
                    (nDistanceSq > nRadiiSubDistSq))
                {
                    // Remove it and feed it to the parent for reinsertion
                    itrTemp = itr;
                    itrTemp++;

                    SphereData *data = (*itr)->m_pData;
                    delete (*itr);
                    itr = m_uNodeChildren.erase(itr);
                    m_pParentNode->addSphereData(data);

                    //itr = itrTemp;
                    updateToFitChildren();
                } else {
                    itr++;
                }
            }
        }

        updateToFitChildren();
    }

    if (!m_bRootNode) {
        if (m_nRadius > m_nMaxRadius) {
            m_nRadius = m_nMaxRadius;
        } else if (m_nRadius < m_nMinRadius) {
            m_nRadius = m_nMinRadius;
        }
    }
}

bool SphereNode::addSphereData(SphereData *_data)
{
    //SphereNode *pBestNode = getBestFitNode(_data);
    SphereNode *pNewNode;
    vector<SphereNode *>::iterator itr;
    gmtl::Vec3f nPosition;
    float nRadiusSq = pow(m_nRadius, 2.0f);
    float nDistanceSq, nOtherRadiusSq;
    nDistanceSq = gmtl::lengthSquared<float, 3>(_data->getPosition() - m_nPosition);
    nOtherRadiusSq = pow(m_nMinRadius, 2.0f);

    //if (pBestNode == this) {
    // Check if this data will fit in our sphere
    if (!m_bDataNode && (nDistanceSq + nOtherRadiusSq < nRadiusSq) || m_bRootNode) {
        pNewNode = new SphereNode(m_nMinRadius, m_nMaxRadius, _data);

        // Can it even fit?
        if ((m_uNodeChildren.size() >= m_nMaxBucketSize)) {
            // Won't fit, we're going to have a create an internal data node around some of our existing children
            // Drop a sphere node around all of our children, and drop the new data node and sphere node into us
            // and remove our existing children
            f_printf(" Node is full... creating new internal node...\n");

            SphereNode *newNode = new SphereNode(m_nMinRadius, m_nMaxRadius, false, m_nMaxBucketSize);
            SphereNode *pNode1, *pNode2;
            float nDistance;

            nDistance = getTwoClosestChildren(pNode1, pNode2);
            if (nDistance > 0.0f && nDistance < m_nMaxRadius) {
                newNode->addSphereNode(pNode1);
                newNode->addSphereNode(pNode2);
                newNode->updateToFitChildren();

                // Add then remove the two
                for (itr = m_uNodeChildren.begin(); itr != m_uNodeChildren.end(); /* Do nothing */) {
                    if (*itr == pNode1 || *itr == pNode2) {
                        itr = m_uNodeChildren.erase(itr);
                    } else {
                        itr++;
                    }
                }
            } else if (nDistance >= m_nMaxRadius) {
                // Our distances between our two closest children is too great for a node to handle
                //  we need to send some to the parent to deal with

                int dummy = 0;
            }

            addSphereNode(newNode);
            
        }

        addSphereNode(pNewNode);

        return true;
    } else {
        // Else send it to our parent
        return m_pParentNode->addSphereData(_data);
    }
}

void SphereNode::addSphereNode(SphereNode *_node)
{
    vector<SphereNode *>::iterator itr;

    if (m_bDataNode)
        f_printf("Warning: Adding a node to a data node!\n");

    // Simply add the internal node to us
    m_uNodeChildren.push_back(_node);
    _node->m_pParentNode = this;
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
void SphereNode::updateToFitChildren()
{
    vector<SphereNode*>::iterator itr;
    float       nMin[3] = {HUGE_VAL};
    float       nMax[3] = {0.0f};
    gmtl::Vec3f nPosition;
    float       nRadius;
    
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

        //if (m_bRootNode && m_nRadius < 500.0f)
        //    m_nRadius = 500.0f;

        m_nPosition[gmtl::Xelt] = (nMax[gmtl::Xelt] + nMin[gmtl::Xelt]) / 2.0f;
        m_nPosition[gmtl::Zelt] = (nMax[gmtl::Zelt] + nMin[gmtl::Zelt]) / 2.0f;
    }
}

float SphereNode::getTwoClosestChildren(SphereNode *& _node1, SphereNode *& _node2)
{
    vector<SphereNode*>::iterator itr, itr2;
    float nDistanceSq = HUGE_VAL, nTemp;
    gmtl::Vec3f nTempVec;

    if (m_uNodeChildren.size() < 2)
        return 0.0f;

    for (itr = m_uNodeChildren.begin(); itr != m_uNodeChildren.end(); itr++) {
        for (itr2 = m_uNodeChildren.begin(); itr2 != m_uNodeChildren.end(); itr2++) {
            if (*itr == *itr2)
                continue;

            nTempVec = (*itr)->getPosition() - (*itr2)->getPosition();
            nTemp = gmtl::lengthSquared(nTempVec);
            if (nTemp < nDistanceSq) {
                nDistanceSq = nTemp;
                _node1 = *itr;
                _node2 = *itr2;
            }
        }
    }

    return sqrt(nDistanceSq);
}