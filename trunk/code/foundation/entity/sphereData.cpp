#include "sphereData.h"

using namespace Foundation;
using namespace Foundation::Entities;


SphereData::SphereData()
{
}

gmtl::Vec3f SphereData::getPosition() const
{
    return m_nPosition;
}

float SphereData::getRadius()
{
    return m_nRadius;
}
