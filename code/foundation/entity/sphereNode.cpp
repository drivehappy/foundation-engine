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

    //f_printf("^^ Added new SphereNode (%p): DataNode: %i\n", this, m_bDataNode);
}

SphereNode::SphereNode(const float _nMinRadius, const float _nMaxRadius, SphereData *_pData)
{
    m_sGraphicID = new char[12];
    sprintf(m_sGraphicID, "%p", this);

    m_nMinRadius = _nMinRadius;
    m_nMaxRadius = _nMaxRadius;
    m_nRadius = m_nMinRadius;
    m_bRootNode = false;

    m_nMaxBucketSize = 0xDEADC0DE;
    m_bDataNode = true;
    m_pData = _pData;

    vector<gmtl::Vec3f> Points;
    Points.push_back(gmtl::Vec3f(0, 0, 0));
    Ogre::String sLineID = m_sGraphicID + Ogre::String("_PARENT_LINE");
    Graphic::GraphicManager::getSingleton().addLine("SceneManager0", sLineID.c_str(), Points, 1.0f, 0.0f, 0.0f);

    //f_printf("^^ Added new SphereNode (%p): DataNode: %i\n", this, m_bDataNode);
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

    //f_printf("^^ Added new SphereNode (%p): DataNode: %i\n", this, m_bDataNode);
}

SphereNode::~SphereNode()
{
    Ogre::String sLineID = m_sGraphicID + Ogre::String("_PARENT_LINE");
    Graphic::GraphicManager::getSingleton().clearLine("SceneManager0", sLineID.c_str());
    Graphic::GraphicManager::getSingleton().clearCircle("SceneManager0", m_sGraphicID);    
    delete m_sGraphicID;
    m_pData = NULL;
}


void SphereNode::update()
{
    vector<SphereNode *>::iterator  itr;
    vector<SphereNode *>            vecChildDelete;
    vector<SphereNode *>            vecInterior, vecData;
    gmtl::Vec3f                     nPosition;

    // If we're a data node update our node position to our data
    if (m_bDataNode) {
        m_nPosition = m_pData->getPosition();
    } else {
        updateToFitChildren();

        // If we're the root node try to jump start an internal node
        if (m_uNodeChildren.size() > m_nMaxBucketSize) {
            SphereNode *internalNode = createInternalNode();
            if (internalNode) {
                addSphereNode(internalNode);
            }
        }

        updateToFitChildren();

        // Move through our children and update and
        // Generate an interior node list and a data node list
        for (size_t i = 0; i < m_uNodeChildren.size(); /* Do Nothing */) {
            // First check to see if the child is an interior node with no children, if so remove
            if (!m_uNodeChildren[i]->m_bDataNode && m_uNodeChildren[i]->m_uNodeChildren.size() <= 0) {
                //f_printf("%p Removing empty node %p\n", this, m_uNodeChildren[i]);

                delete m_uNodeChildren[i];
                m_uNodeChildren.erase(m_uNodeChildren.begin() + i);
            } 

            // Check if the child is interior node with 1 child, if so remove
            else if ((!m_uNodeChildren[i]->m_bDataNode && 
                      m_uNodeChildren[i]->m_uNodeChildren.size() == 1))
            {
                f_printf("%p Removing useless node %p\n", this, m_uNodeChildren[i]);

                SphereNode *pChild = m_uNodeChildren[i]->m_uNodeChildren[0];
                delete m_uNodeChildren[i];
                m_uNodeChildren.erase(m_uNodeChildren.begin() + i);
                pChild->m_pParentNode = this;
                m_uNodeChildren.push_back(pChild);
                i = 0;
            }

            // Child is good, go ahead and update
            else {
                m_uNodeChildren[i]->update();
                i++;
            }
        }

        // Again, check the existing ones now
        for (size_t i = 0; i < m_uNodeChildren.size(); i++) {
            if (m_uNodeChildren[i]->m_bDataNode)
                vecData.push_back(m_uNodeChildren[i]);
            else
                vecInterior.push_back(m_uNodeChildren[i]);
        }


        // See if we can fit any of our data nodes into our interior nodes
        for (size_t j = 0; j < vecInterior.size(); j++) {
            for (size_t i = 0; i < vecData.size(); /* nothing */) {
                SphereNode *pNodeData = (*(vecData.begin() + i));
                SphereNode *pNodeIntr = (*(vecInterior.begin() + j));

                if (pNodeIntr->canAcceptData(pNodeData->m_pData)) {
                    //f_printf("Moving data node into lower interior node...\n");

                    pNodeIntr->addSphereNode(pNodeData);
                    removeSphereNode(pNodeData);
                    vecData.erase(vecData.begin() + i);
                } else {
                    i++;
                }
            }
        }
        vecInterior.clear();
        vecData.clear();

        updateToFitChildren();

        if (!m_bRootNode) {
            checkOutsideChildren();
        }

        updateToFitChildren();
    }
}

void SphereNode::checkOutsideChildren()
{
    vector<SphereNode *>::iterator  itr;
    vector<SphereNode *>            vecChildDelete;
    vector<SphereNode *>            vecInterior, vecData;
    gmtl::Vec3f                     nPosition;
    float                           nDistanceSq;
    float                           nRadiiSubDistSq;

    // Move through our updated children vector and determine if any lie outside our radius
    for (itr = m_uNodeChildren.begin(); itr != m_uNodeChildren.end(); itr++) {
        nDistanceSq = gmtl::length<float, 2>(
            gmtl::Vec2f((*itr)->m_nPosition[0], (*itr)->m_nPosition[2]) - 
                gmtl::Vec2f(m_nPosition[0], m_nPosition[2]));

        nRadiiSubDistSq = m_nRadius - (*itr)->m_nRadius;
        //nRadiiSumDistSq = m_nRadius + (*itr)->m_nRadius;
        //nRadiiDistSq = m_nRadius;
        
        // Determine 0-multiple nodes exiting our maxed out radius
        if (nDistanceSq > nRadiiSubDistSq) {
            //f_printf("Child outside (%p): Distance: %f\n        Radius: %f Position: %f, %f\n", (*itr), nDistanceSq, m_nRadius, m_nPosition[0], m_nPosition[2]);
            // Add it to our delete list
            vecChildDelete.push_back((*itr));
        }
    }

    // If we have anything to possibly delete
    if (vecChildDelete.size() > 0) {
        SphereNode *pBestNode = NULL;
        float nCurrentDistance = HUGE_VAL;

        // First, determine our average child positions
        gmtl::Vec3f nAverageChildPos = determineAverageChildPositions();
        gmtl::Vec3f nDiff;

        //f_printf("%p (Parent %p): Found %i children to possibly delete\n", this, m_pParentNode, vecChildDelete.size());

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

        //
        // Remove the best node from our list and send it to our parent
        //
        vector<SphereData *>            dataList;
        vector<SphereData *>::iterator  itrData;

        if (!pBestNode->m_bDataNode) {
            //f_printf("%%%% Node is Internal Node (%i Children)\n", _node->m_uNodeChildren.size());
            for (itr = pBestNode->m_uNodeChildren.begin(); itr != pBestNode->m_uNodeChildren.end(); itr++) {
                // Steal the node's child and put it into our parent
                if (m_pParentNode)
                    m_pParentNode->addSphereNode((*itr));
                else
                    addSphereNode((*itr));

                //f_printf("$$ Moving removed node's child %p to parent %p (%p)\n", (*itr), m_pParentNode, (*itr)->m_pParentNode);
            }
        } else {
            //f_printf("%%%% Node is Data Node\n");
            dataList.push_back(pBestNode->m_pData);
        }

        // Remove
        removeSphereNode(pBestNode);
        delete pBestNode;

        // Loop through the data and add it to our parent if not root
        for (itrData = dataList.begin(); itrData != dataList.end(); itrData++) {
            if (m_pParentNode)
                m_pParentNode->addSphereData((*itrData));
            else
                addSphereData((*itrData));
        }
    }
}

bool SphereNode::canAcceptData(SphereData *_data)
{
    float nDistance;
    
    nDistance = gmtl::length<float, 3>(_data->getPosition() - m_nPosition);

    // Check if this data will fit in our sphere
    if ( (!m_bDataNode && (nDistance + (m_nMinRadius * 2.0f) < m_nMaxRadius) && 
        (m_uNodeChildren.size() < m_nMaxBucketSize) ) || m_bRootNode)
    {
        return true;
    }

    return false;
}

SphereNode* SphereNode::getBestFitChild(SphereData *_uData)
{
    vector<SphereNode *>::iterator itr;
    SphereNode *pBestChild = NULL, *pTempBest;
    gmtl::Vec3f vecDiff;
    float nBestDistance = HUGE_VAL, nDistance;

    vecDiff = m_nPosition - _uData->getPosition();
    nDistance = gmtl::length(vecDiff);

    if ((!m_bDataNode && nDistance > m_nMinRadius && nDistance < m_nMaxRadius && m_uNodeChildren.size() < m_nMaxBucketSize) || m_bRootNode) {
        // Assume we're the best fit until we check our children
        pBestChild = this;
        nBestDistance = nDistance;

        for (itr = m_uNodeChildren.begin(); itr != m_uNodeChildren.end(); itr++) {
            pTempBest = (*itr)->getBestFitChild(_uData);

            if (pTempBest && pBestChild) {
                vecDiff = (pTempBest->m_nPosition - pBestChild->m_nPosition);
                nDistance = gmtl::length(vecDiff);

                if (nDistance < nBestDistance) {
                    pBestChild = pTempBest;
                    nBestDistance = nDistance;
                }
            }
        }
    }

    return pBestChild;
}

bool SphereNode::addSphereData(SphereData *_data)
{
    SphereNode *pBestNode, *pNewNode, *newNode;
    vector<SphereNode *>::iterator itr;
    gmtl::Vec3f nPosition;
    float nRadiusSq = pow(m_nRadius, 2.0f);
    float nDistanceSq, nOtherRadiusSq;
    nDistanceSq = gmtl::lengthSquared<float, 3>(_data->getPosition() - m_nPosition);
    nOtherRadiusSq = pow(m_nMinRadius, 2.0f);

    pBestNode = getBestFitChild(_data);

    if (pBestNode) {
        pNewNode = new SphereNode(m_nMinRadius, m_nMaxRadius, _data);

        if ((m_uNodeChildren.size() >= m_nMaxBucketSize)) {
            newNode = createInternalNode();

            if (newNode)
                pBestNode->addSphereNode(newNode);
        }

        pBestNode->addSphereNode(pNewNode);
        pBestNode->updateToFitChildren();

        f_printf("Best Fit Node Found: %p for %p\n", pBestNode, pNewNode);

        return true;
    } else {
        f_printf("NOT FOUND, PUSHING TO PARENT\n");

        // Push it higher
        return m_pParentNode->addSphereData(_data);
    }
}

SphereNode* SphereNode::createInternalNode()
{
    vector<SphereNode *>::iterator itr;
    SphereNode *pNode1, *pNode2;
    SphereNode *newNode = NULL;
    float nDistance;

    nDistance = getTwoClosestChildren(pNode1, pNode2);

    // Only create this new node if the 2 closest children are below max radius
    if (nDistance > 0.0f && nDistance + (m_nMinRadius * 2) < m_nMaxRadius) {
        newNode = new SphereNode(m_nMinRadius, m_nMaxRadius, false, m_nMaxBucketSize);
        newNode->addSphereNode(pNode1);
        newNode->addSphereNode(pNode2);

        // Then remove the two from our node
        for (itr = m_uNodeChildren.begin(); itr != m_uNodeChildren.end(); ) {
            if (*itr == pNode1 || *itr == pNode2) {
                itr = m_uNodeChildren.erase(itr);
            } else {
                itr++;
            }
        }

        f_printf("@@ Created new internal node: %p\n", newNode);            
    }

    return newNode;
}

void SphereNode::addSphereNode(SphereNode *_node)
{
    vector<SphereNode *>::iterator itr;

    if (m_bDataNode)
        f_printf("Warning: Adding a node to a data node!\n");

    //f_printf("%p (Parent %p) Adding a Sphere Node: %p\n", this, m_pParentNode, _node);

    // Simply add the internal node to us
    m_uNodeChildren.push_back(_node);
    _node->m_pParentNode = this;

    updateToFitChildren();
}

void SphereNode::removeSphereNode(SphereNode *_node)
{
    vector<SphereNode *>::iterator  itr;
    vector<SphereData *>            dataList;
    vector<SphereData *>::iterator  itrData;

    if (!m_bDataNode) {
        for (size_t i = 0; i < m_uNodeChildren.size(); i++) {
            // Search for the node in our children
            if (m_uNodeChildren[i] == _node) {
                f_printf("## Removed node %p from %p, Contained %i Data Children, Reinserting...\n", _node, this, dataList.size())

                m_uNodeChildren.erase(m_uNodeChildren.begin() + i);

                updateToFitChildren();

                break;
            }
        }
    }
}

void SphereNode::debugRender(const char* _sSceneManagerName, bool _bRecursive)
{
    gmtl::Vec3f nPosition = m_nPosition;
    
    nPosition[gmtl::Yelt] = 20.0f;

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

        m_nRadius = gmtl::length(nRadiusDistance) / 2.0f;

        if (!m_bRootNode) {
            if (m_nRadius > m_nMaxRadius) {
                m_nRadius = m_nMaxRadius;
            } else if (m_nRadius < m_nMinRadius) {
                m_nRadius = m_nMinRadius;
            }
        }

        m_nPosition[gmtl::Xelt] = (nMax[gmtl::Xelt] + nMin[gmtl::Xelt]) / 2.0f;
        m_nPosition[gmtl::Yelt] = 20.0f;
        m_nPosition[gmtl::Zelt] = (nMax[gmtl::Zelt] + nMin[gmtl::Zelt]) / 2.0f;

        //f_printf("%p Updated to %f, %f\n", this, m_nPosition[0], m_nPosition[2]);
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

            //if (!(*itr)->m_bDataNode || !(*itr2)->m_bDataNode)
            //    continue;

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

void SphereNode::dump(int _level)
{
    vector<SphereNode*>::iterator   itr;

    for (itr = m_uNodeChildren.begin(); itr != m_uNodeChildren.end(); itr++) {
        for (int i = 0; i < _level; i++)
            f_printf("  ");

        if ((*itr)->m_bDataNode) {
            f_printf("Data Node (%p): P: %f, %f - R: %f\n", (*itr), (*itr)->m_nPosition[0], (*itr)->m_nPosition[2], (*itr)->m_nRadius);
        } else {
            f_printf("Internal Node (%p): P: %f, %f - R: %f\n", (*itr), (*itr)->m_nPosition[0], (*itr)->m_nPosition[2], (*itr)->m_nRadius);
            (*itr)->dump(_level + 1);
        }
    }
}

