#if !defined(__F_ENTITYGRAPHICS_H_)
#define __F_ENTITYGRAPHICS_H_

// Foundation
#include "graphics/gfxSystem.h"

using namespace Foundation::Graphic;


namespace Foundation
{
    namespace Entities
    {
        /** Handles graphical components of an entity.
		 */
        class EntityGraphic
        {
        public:
            EntityGraphic(const char *_sSceneManager, const char *_sMeshId);
            ~EntityGraphic();

            void setMesh(const char *_sMeshName);
            void setScale(gmtl::Vec3f _nScale);
            void setRotation(gmtl::Quatf _nRotation);
            void setPosition(gmtl::Vec3f _nPosition);
            void setMaterial(const char *_sMaterialName);

            gmtl::Vec3f getPosition();

        private:
            GraphicManager  *m_pGraphicManager;
            const char      *m_sMeshId;
            const char      *m_sSceneManager;
        };   
    };
};


#endif  // __F_ENTITYGRAPHICS_H_
