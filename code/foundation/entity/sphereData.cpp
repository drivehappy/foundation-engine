#include "sphereData.h"

using namespace Foundation;
using namespace Foundation::Entities;


SphereData::SphereData()
{
}

gmtl::Vec3f SphereData::getPosition(void)
{
    return m_nPosition;
}

float SphereData::getRadius(void)
{
    return m_nRadius;
}
