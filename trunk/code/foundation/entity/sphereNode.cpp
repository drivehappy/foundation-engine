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

    vector<gmtl::Vec3f> Points;
    Points.push_back(gmtl::Vec3f(0, 0, 0));
    Ogre::String sLineID = m_sGraphicID + Ogre::String("_PARENT_LINE");
    Graphic::GraphicManager::getSingleton().addLine("SceneManager0", sLineID.c_str(), Points, 1.0f, 0.0f, 0.0f);
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

    vector<gmtl::Vec3f> Points;
    Points.push_back(gmtl::Vec3f(0, 0, 0));
    Ogre::String sLineID = m_sGraphicID + Ogre::String("_PARENT_LINE");
    Graphic::GraphicManager::getSingleton().addLine("SceneManager0", sLineID.c_str(), Points, 1.0f, 0.0f, 0.0f);
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

    vector<gmtl::Vec3f> Points;
    Points.push_back(gmtl::Vec3f(0, 0, 0));
    Ogre::String sLineID = m_sGraphicID + Ogre::String("_PARENT_LINE");
    Graphic::GraphicManager::getSingleton().addLine("SceneManager0", sLineID.c_str(), Points, 1.0f, 0.0f, 0.0f);
}

SphereNode::~SphereNode()
{
    Ogre::String sLineID = m_sGraphicID + Ogre::String("_PARENT_LINE");
    Graphic::GraphicManager::getSingleton().destroyLine("SceneManager0", sLineID.c_str());
    Graphic::GraphicManager::getSingleton().destroyCircle("SceneManager0", m_sGraphicID);    
    delete m_sGraphicID;

}


void SphereNode::update()
{
    vector<SphereNode *>::iterator  itr;
    vector<SphereNode *>            vecChildDelete;
    //float       nMin[3] = {HUGE_VAL};
    //float       nMax[3] = {0.0f};
    gmtl::Vec3f nPosition;
    float       nDistanceSq;
    float       nRadiiSumDistSq, nRadiiSubDistSq, nRadiiDistSq;

    // If we're a data node update our node position to our data
    if (m_bDataNode) {
        m_nPosition = m_pData->getPosition();
    } else {
        // Move through our children and update
        for (size_t i = 0; i < m_uNodeChildren.size(); /* Do Nothing */) {
            // First check to see if the child is an interior node with no children, if so remove
            if (!m_uNodeChildren[i]->m_bDataNode && m_uNodeChildren[i]->m_uNodeChildren.size() <= 0) {
                f_printf("%p Removing empty node %p\n", this, m_uNodeChildren[i]);

                delete m_uNodeChildren[i];
                m_uNodeChildren.erase(m_uNodeChildren.begin() + i);
            } else {
                m_uNodeChildren[i]->update();
                i++;
            }
        }

        updateToFitChildren();

        if (!m_bRootNode) {
            // Move through our updated children vector and determine
            //  if any lie outside our radius
            for (itr = m_uNodeChildren.begin(); itr != m_uNodeChildren.end(); /* Do nothing */) {
                nDistanceSq = gmtl::length<float, 3>((*itr)->m_nPosition - m_nPosition);
                nRadiiSumDistSq = (*itr)->m_nRadius + m_nRadius;
                nRadiiSubDistSq = (*itr)->m_nRadius - m_nRadius;
                nRadiiDistSq = m_nRadius;
                
                // Since our sphere bounds the children so nicely, when one exits due to sphere's max radius
                //  another on another side is exiting too, so we need to determine the outsider in the group
                //  and only remove that one
                if ((nDistanceSq > nRadiiSumDistSq) ||
                    (nDistanceSq > nRadiiDistSq) ||
                    (nDistanceSq > nRadiiSubDistSq))
                {
                    f_printf("Child outside: Distance: %f\n        Radius: %f Position: %f, %f\n", nDistanceSq, m_nRadius, m_nPosition[0], m_nPosition[2]);
                    // Add it to our delete list
                    vecChildDelete.push_back((*itr));
                }

                itr++;
            }

            // If we have anything to possibly delete
            if (vecChildDelete.size() > 0) {
                SphereNode *pBestNode = NULL;
                float nCurrentDistance = HUGE_VAL;

                // First, determine our average child positions
                gmtl::Vec3f nAverageChildPos = determineAverageChildPositions();
                gmtl::Vec3f nDiff;

                f_printf("%p (Parent %p): Found %i children to possibly delete\n", this, m_pParentNode, vecChildDelete.size());

                // Initially start with our first one
                pBestNode = vecChildDelete[0];
                nDiff = pBestNode->m_nPosition - nAverageChildPos;
                nCurrentDistance = gmtl::length(nDiff);

                // Move through our delete list and determine which node is better to remove
                for (size_t i = 1; i < vecChildDelete.size(); i++) {
                    nDiff = vecChildDelete[i]->m_nPosition - nAverageChildPos;
                    if (gmtl::length(nDiff) < nCurrentDistance) {
                        nCurrentDistance = gmtl::length(nDiff);
                        pBestNode = vecChildDelete[i];
                    }
                }

                // Remove the best node from our list
                removeSphereNode(pBestNode);
                vecChildDelete.clear();
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

    // Check if this data will fit in our sphere
    if ((!m_bDataNode && (nDistanceSq + nOtherRadiusSq < nRadiusSq)) || m_bRootNode) {
        pNewNode = new SphereNode(m_nMinRadius, m_nMaxRadius, _data);

        // Can it even fit?
        if ((m_uNodeChildren.size() >= m_nMaxBucketSize)) {
            // Won't fit, we're going to have a create an internal data node around some of our existing children
            // Drop a sphere node around all of our children, and drop the new data node and sphere node into us
            // and remove our existing children
            SphereNode *newNode = new SphereNode(m_nMinRadius, m_nMaxRadius, false, m_nMaxBucketSize);
            SphereNode *pNode1, *pNode2;
            float nDistance;

            nDistance = getTwoClosestChildren(pNode1, pNode2);
            if (nDistance > 0.0f && nDistance < m_nMaxRadius) {
                newNode->addSphereNode(pNode1);
                newNode->addSphereNode(pNode2);

                // Then remove the two from our node
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

                //if (m_pParentNode)
                //    m_pParentNode->addSphereData(_data);
            }

            addSphereNode(newNode);
            updateToFitChildren();

            f_printf(" Node is full: Created new internal node: %p\n", newNode);            
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

    f_printf("%p (Parent %p) Adding a Sphere Node: %p\n", this, m_pParentNode, _node);

    // Simply add the internal node to us
    m_uNodeChildren.push_back(_node);
    _node->m_pParentNode = this;

    updateToFitChildren();
}

void SphereNode::removeSphereNode(SphereNode *_node)
{
    vector<SphereNode *>::iterator  itr;
    vector<SphereData *>            dataList;

    if (!m_bDataNode) {
        for (size_t i = 0; i < m_uNodeChildren.size(); i++) {
            if (m_uNodeChildren[i] == _node) {
                for (itr = _node->m_uNodeChildren.begin(); itr != _node->m_uNodeChildren.end(); itr++) {
                    if ((*itr)->m_bDataNode) {
                        dataList.push_back((*itr)->m_pData);
                    } else {
                        removeSphereNode((*itr));
                    }
                }

                delete _node;
                m_uNodeChildren.erase(m_uNodeChildren.begin() + i);

                for (size_t j = 0; j < dataList.size(); j++) {
                    addSphereData((*(dataList.begin() + i)));
                }
                dataList.clear();

                break;
            }
        }

        updateToFitChildren();
    }
}

void SphereNode::debugRender(const char* _sSceneManagerName, bool _bRecursive)
{
    gmtl::Vec3f nPosition = m_nPosition;
    
    nPosition[gmtl::Yelt] = 15.0f;

    // Do Render Circle
    Graphic::GraphicManager::getSingleton().updateCircle(_sSceneManagerName, m_sGraphicID, nPosition, m_nRadius, gmtl::Yelt);

    // Render a line from our center to our parent's center
    if (m_pParentNode) {
        vector<gmtl::Vec3f> Points;
        Points.push_back(m_nPosition);
        Points.push_back(m_pParentNode->m_nPosition);
        
        Ogre::String sLineID = m_sGraphicID + Ogre::String("_PARENT_LINE");
        Graphic::GraphicManager::getSingleton().updateLine("SceneManager0", sLineID.c_str(), Points);
    }

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

// Centers the position and radius around children
void SphereNode::updateToFitChildren()
{
    vector<SphereNode*>::iterator itr;
    gmtl::Vec3f nMin, nMax;
    gmtl::Vec3f nPosition;
    float       nRadius;

    nMax[gmtl::Xelt] = -HUGE_VAL;
    nMax[gmtl::Zelt] = -HUGE_VAL;
    nMin[gmtl::Xelt] = HUGE_VAL;
    nMin[gmtl::Zelt] = HUGE_VAL;
    
    if (m_uNodeChildren.size() == 0) { 
        m_nPosition = gmtl::Vec3f(0, 0, 0);
        m_nRadius = 0.0f;
    } else {
        // Traverse each child and determine our min/max
        for (itr = m_uNodeChildren.begin(); itr != m_uNodeChildren.end(); itr++) {
            nPosition = (*itr)->m_nPosition;
            nRadius = (*itr)->m_nRadius;

            if (nPosition[gmtl::Xelt] - nRadius < nMin[gmtl::Xelt])
                nMin[gmtl::Xelt] = nPosition[gmtl::Xelt] - nRadius;
            if (nPosition[gmtl::Xelt] + nRadius > nMax[gmtl::Xelt])
                nMax[gmtl::Xelt] = nPosition[gmtl::Xelt] + nRadius;

            if (nPosition[gmtl::Zelt] - nRadius < nMin[gmtl::Zelt])
                nMin[gmtl::Zelt] = nPosition[gmtl::Zelt] - nRadius;
            if (nPosition[gmtl::Zelt] + nRadius > nMax[gmtl::Zelt])
                nMax[gmtl::Zelt] = nPosition[gmtl::Zelt] + nRadius;
        }

        // Compute the radius and position
        gmtl::Vec2f nRadiusDistance = gmtl::Vec2f(
            nMax[gmtl::Xelt] - nMin[gmtl::Xelt],
            nMax[gmtl::Zelt] - nMin[gmtl::Zelt]);

        m_nRadius = (gmtl::length(nRadiusDistance) / 2.0f);

        m_nPosition[gmtl::Xelt] = (nMax[gmtl::Xelt] + nMin[gmtl::Xelt]) / 2.0f;
        m_nPosition[gmtl::Zelt] = (nMax[gmtl::Zelt] + nMin[gmtl::Zelt]) / 2.0f;

        f_printf("%p Updated to %f, %f\n", this, m_nPosition[0], m_nPosition[2]);
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

gmtl::Vec3f SphereNode::determineAverageChildPositions(void)
{
    vector<SphereNode*>::iterator   itr;
    gmtl::Vec3f                     nAveragePosition = gmtl::Vec3f(0, 0, 0);

    for (itr = m_uNodeChildren.begin(); itr != m_uNodeChildren.end(); itr++) {
        nAveragePosition += (*itr)->m_nPosition;
    }

    nAveragePosition /= m_uNodeChildren.size();

    return nAveragePosition;
}

