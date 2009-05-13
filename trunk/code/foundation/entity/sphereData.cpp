#include "sphereData.h"

using namespace Foundation;
using namespace Foundation::Entities;


SphereData::SphereData()
{
    m_nPosition = gmtl::Vec3f(0, 0, 0);
    m_nRadius = 0.0f;
}

gmtl::Vec3f SphereData::getPosition() const
{
    return m_nPosition;
}

void SphereData::setPosition(const gmtl::Vec3f _nPosition)
{
    m_nPosition = _nPosition;
}

float SphereData::getRadius()
{
    return m_nRadius;
}

void SphereData::setRadius(const float _nRadius)
{
    m_nRadius = _nRadius;
}
