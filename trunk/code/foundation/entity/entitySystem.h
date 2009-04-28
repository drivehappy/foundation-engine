#if !defined(__F_ENTITYSYSTEM_H_)
#define __F_ENTITYSYSTEM_H_

// Foundation
#include "kernel/kernel.h"
#include "entity/entityGraphics.h"

// STL
#include <vector>
using std::vector;


namespace Foundation
{
    namespace Entities
    {
        typedef unsigned int EntityIdType;

        /** Base entity class.
         */
        class Entity
        {
        public:
	        /** Get the ID of the Entity.
		        @return
			        Unique ID of the Entity.
	         */
	        EntityIdType getId();

            /** */
            char* getName();
            /** */
            void setName(const char *_sName);

            /** */
            void setGraphics(EntityGraphic *_pEntityGraphic);

            /** */
            EntityGraphic* getGraphics();

        //protected:
	        /** Constructor.
		        @param
			        Unique ID to supply to the Entity, this cannot be changed later.
	         */
	        Entity(EntityIdType _nId, const char *_sName);

        private:
	        /** Unique Id for each entity. */
	        EntityIdType	m_nId;
	        char			m_sName[FOUNDATION_MAXSTRINGLEN];

            /** */
            EntityGraphic   *m_uGraphics;

	            /** Allow only our EntityManager to call our constructor. */
	            friend class	EntityManager;
            };

        /** Factory/Singleton class for managing entities.
         */
        class EntityManager
        {
        public:
	        static EntityManager& getSingleton();

	        /** Task wrapper for updating the entities.
            @param
                The EntityManager to use, since this is a singleton, the singleton
                instance can be passed in.
            @param
                The argument to pass in, generally the elapsed time.
             */
            inline static void doTaskUpdate_Wrapper(void *_pClass, void *_pArgs)
                { reinterpret_cast<EntityManager*>(_pClass)->doTaskUpdate(_pArgs); };

	            /** Retrieve our task, useful for exposing the class to Python.
             */
            Task *getTask() const
		        { return m_pTask; };

	        /** Create and add an entity to the manager.
	         */
	        Entity* addEntity(const char *_sName);

        protected:
	        /** Force the constructor through getSingleton.
	         */
	        EntityManager();
	        /** Force the copy-constructor through getSingleton.
	         */
	        EntityManager(const EntityManager&);
	        /** Force the assignment-constructor through getSingleton.
	         */
	        EntityManager& operator=(const EntityManager&);
	        /**
	         */
	        ~EntityManager();

        private:
	        /** Retrieve an available unique Id.
		        @return
			        A unique Id, used internally for creating entities.
	         */
	        EntityIdType		getAvailableUniqueId();


	        // Tasks
	        static Task			*m_pTask;
	        static void			*doTaskUpdate(void *_pArgs);

	        /** A list of entities */
	        vector<Entity *>	m_uEntityList;
        };
    };
};

#endif	// __F_ENTITYSYSTEM_H_
