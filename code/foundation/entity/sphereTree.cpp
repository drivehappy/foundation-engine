#include "sphereTree.h"

using namespace Foundation;
using namespace Foundation::Entity;

SphereTree::SphereTree()
{
    m_nMinRadius = 1.0f;
    m_nMaxRadius = 50.0f;

    m_pRoot = new SphereNode(10);
}

SphereTree::~SphereTree()
{
}

void SphereTree::addData(const SphereData & _data)
{
    // Determine if there's an existing sphere to place this data
    /// If so, determine if any of the children are best to contain it
    /// If not, create a new node to hold this data
    
}

void SphereTree::removeData(SphereNode* _iterator)
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
