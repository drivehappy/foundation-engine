#include "entity/entityGraphics.h"

using namespace Foundation::Entities;
using namespace Foundation::Graphic;

EntityGraphic::EntityGraphic(const char *_sSceneManager, const char *_sMeshId)
{
    m_pGraphicManager = &GraphicManager::getSingleton();
    m_sSceneManager = new char[FOUNDATION_MAXSTRINGLEN];
    m_sMeshId = new char[FOUNDATION_MAXSTRINGLEN];

    if (strlen(_sSceneManager) > FOUNDATION_MAXSTRINGLEN) {
        f_printf("[EntityGraphic::EntityGraphic] Error: SceneManager name exceeds string length.\n");
        return;
    }

    if (strlen(_sMeshId) > FOUNDATION_MAXSTRINGLEN) {
        f_printf("[EntityGraphic::EntityGraphic] Error: MeshId exceeds string length.\n");
        return;
    }

    strcpy((char *)m_sSceneManager, _sSceneManager);
    strcpy((char *)m_sMeshId, _sMeshId);
    m_pGraphicManager->addMesh(m_sSceneManager, m_sMeshId, "cube.mesh", 0);
}

EntityGraphic::~EntityGraphic()
{
    if (m_sMeshId) {
        delete m_sMeshId;
        m_sMeshId = NULL;
    }
    if (m_sSceneManager) {
        delete m_sSceneManager;
        m_sSceneManager = NULL;
    }

    m_pGraphicManager = NULL;
}

void EntityGraphic::setMesh(const char *_sMeshName)
{
    m_pGraphicManager->removeMesh(m_sSceneManager, m_sMeshId);
    m_pGraphicManager->addMesh(m_sSceneManager, m_sMeshId, _sMeshName, 0);
}

void EntityGraphic::setScale(gmtl::Vec3f _nScale)
{
    m_pGraphicManager->setMeshScale(m_sSceneManager, m_sMeshId, _nScale);
}

void EntityGraphic::setRotation(gmtl::Quatf _nRotation)
{
    m_pGraphicManager->setMeshRotation(m_sSceneManager, m_sMeshId, _nRotation);
}

void EntityGraphic::setPosition(gmtl::Vec3f _nPosition)
{
    m_pGraphicManager->setMeshPosition(m_sSceneManager, m_sMeshId, _nPosition);
}

void EntityGraphic::setMaterial(const char *_sMaterialName)
{
    m_pGraphicManager->setMeshMaterial(m_sSceneManager, m_sMeshId, _sMaterialName);
}
