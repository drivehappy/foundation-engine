#include "sphereTree.h"

using namespace Foundation;
using namespace Foundation::Entities;

SphereTree::SphereTree()
{
    m_nMinRadius = 20;
    m_nMaxRadius = 100;

    m_pRoot = new SphereNode(m_nMinRadius, m_nMaxRadius, false, 10);
    m_pRoot->m_bRootNode = true;
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

void SphereTree::debugRender(const char* _sSceneManagerName)
{
    m_pRoot->debugRender(_sSceneManagerName, true);
}
