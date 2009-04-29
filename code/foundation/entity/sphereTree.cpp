#include "sphereTree.h"

using namespace Foundation;
using namespace Foundation::Entities;

SphereTree::SphereTree()
{
    m_nMinRadius = 1;
    m_nMaxRadius = 50;

    m_pRoot = new SphereNode(false, 10);
}

SphereTree::~SphereTree()
{
}

vector<SphereData *>::iterator& SphereTree::addData(const SphereData & _data)
{
    vector<SphereData *>::iterator *itr = new vector<SphereData *>::iterator();
    //SphereData *dataCopy = new SphereData(_data);

    // Insert at the end, retrieve the iterator to the data
    m_uDataStore.insert(m_uDataStore.end(), _data);
    
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
}

void SphereTree::debugRender(const char* _sSceneManagerName)
{

}
