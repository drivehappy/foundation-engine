#include "sphereTree.h"

using namespace Foundation;
using namespace Foundation::Entities;

SphereTree::SphereTree()
{
    m_nMinRadius = 20;
    m_nMaxRadius = 200;

    m_pRoot = new SphereNode(m_nMinRadius, m_nMaxRadius, false, 4);
    m_pRoot->m_bRootNode = true;
    m_pRoot->m_pParentNode = NULL;
}

SphereTree::~SphereTree()
{
}

vector<SphereData *>::iterator& SphereTree::addData(SphereData *_data)
{
    f_printf("SphereTree Adding Data... (Pos: %f, %f, %f)\n",
         _data->getPosition()[0], _data->getPosition()[1], _data->getPosition()[2]);

    vector<SphereData *>::iterator *itr = new vector<SphereData *>::iterator();
    //SphereData *dataCopy = new SphereData(_data);

    // Insert at the end, retrieve the iterator to the data
    //m_uDataStore.insert(m_uDataStore.end(), _data);
    
    m_pRoot->addSphereData(_data);
    
    return (*itr);
}

void SphereTree::removeData(vector<SphereData *>::iterator& _iterator)
{
}

unsigned int SphereTree::getChildCount()
{
    return m_pRoot->getChildCount();
}

unsigned int SphereTree::getMaxDepth()
{
    return m_pRoot->getMaxDepth();
}

void SphereTree::update()
{
    m_pRoot->update();
}

/*
SphereNode* SphereTree::getBestFitNode(const SphereData *_uData)
{
    // Determine if this point fits within us
    gmtl::Vec3f nPoint = _uData->getPosition();
    float nRadiusSq = pow(m_nRadius, 2.0f);
    float nDistanceSq, nOtherRadiusSq;
    SphereNode *bestNode = NULL;
    vector<SphereNode *>::iterator itr;

    nDistanceSq = gmtl::lengthSquared<float, 3>(nPoint - m_nPosition);
    nOtherRadiusSq = pow(m_nMinRadius, 2.0f);
    
    if (!m_bDataNode && (nDistanceSq + nOtherRadiusSq < nRadiusSq) || m_bRootNode) {
        // It fits, try out the children now
        for (itr = m_uNodeChildren.begin(); itr != m_uNodeChildren.end(); itr++) {
            bestNode = (*itr)->getBestFitNode(_uData);

            if (bestNode)
                break;
        }

        // We couldn't find a suitable child to accept the data, so we'll take it
        if (!bestNode) {
            bestNode = this;
        }
    } else {
        bestNode = NULL;
    }

    return bestNode;
}
*/

void SphereTree::debugRender(const char* _sSceneManagerName)
{
    m_pRoot->debugRender(_sSceneManagerName, true);
}
