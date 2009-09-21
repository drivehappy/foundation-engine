#if !defined(_PY__F_ENTITYSYSTEM_H_)
#define _PY__F_ENTITYSYSTEM_H_

// Boost
#include <boost/python.hpp>
using namespace boost::python;

// Foundation
#include "foundation.h"
#include "entity/entityGraphics.h"
#include "entity/entitySystem.h"

#include "py_gmtl.h"


namespace Foundation
{
	namespace Entities
	{
        class EntityGraphic_PythonWrapper : public EntityGraphic
        {
        public:
            EntityGraphic_PythonWrapper(const char *_sSceneManager, const char *_sMeshId);

            void setScale(gmtlVector3_PythonWrapper _nScale);
            void setRotation(gmtlQuaternion_PythonWrapper _nRotation);
            void setPosition(gmtlVector3_PythonWrapper _nPosition);
	
            gmtlVector3_PythonWrapper getPosition();
        };

		class Entity_PythonWrapper : public Entity
		{
		public:
			//Entity_PythonWrapper(EntityIdType _nId, const char *_sName);
            void setGraphics(EntityGraphic_PythonWrapper *_pGraphic);
            EntityGraphic_PythonWrapper* getGraphics();

            PyObject* getName();
		};

		class EntityManager_PythonWrapper : public EntityManager
		{
		public:
			EntityManager_PythonWrapper();

			Entity_PythonWrapper* addEntity(const char *_sName);
		};
	}
};

#endif	// _PY__F_ENTITYSYSTEM_H_
