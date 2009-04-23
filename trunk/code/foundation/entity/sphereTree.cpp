#include "sphereTree.h"

using namespace Foundation;
using namespace Foundation::Entity;

SphereTree::SphereTree()
{
}

SphereTree::~SphereTree()
{
}

void SphereTree::addData(const SphereData & _data)
{
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
