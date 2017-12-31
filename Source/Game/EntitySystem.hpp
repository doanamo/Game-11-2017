#pragma once

#include "Precompiled.hpp"
#include "EntityHandle.hpp"

/*
    Entity System

    Manages unique identities of each existing entity. Gives means to identify
    different entities and takes care of their creation/destruction.

    Example usage:
        Game::EntitySystem entitySystem;
    
        EntityHandle entity = entitySystem.CreateEntity();
        {
            // Add components here!
            // Entity remains inactive until the next ProcessCommands() call.
        }
        entitySystem.ProcessCommands();
    
        entitySystem.DestroyEntity(entity);
        {
            // Entity can be referenced until the next ProcessCommands() call.
        }
        entitySystem.ProcessCommands();
*/

namespace Game
{
    // Entity system class.
    class EntitySystem
    {
    public:
        // Private event dispatchers.
        struct EventDispatchers
        {
            Dispatcher<bool(EntityHandle), CollectWhileTrue<bool>> entityFinalize;
            Dispatcher<void(EntityHandle)>                         entityDestroy;
        } eventDispatchers;

    public:
        EntitySystem();
        ~EntitySystem();

        // Creates an entity.
        EntityHandle CreateEntity();

        // Destroys an entity.
        void DestroyEntity(const EntityHandle& entity);

        // Destroys all entities.
        void DestroyAllEntities();

        // Process entity commands.
        void ProcessCommands();

        // Checks if an entity handle is valid.
        bool IsHandleValid(const EntityHandle& entity) const;

        // Returns the number of active entities.
        unsigned int GetEntityCount() const
        {
            return m_entityCount;
        }

    private:
        // Handle flags.
        struct HandleFlags
        {
            enum Type
            {
                // Entity handle has been allocated and is free to use.
                Unused = 0,

                // Entity handle has been created and can be used.
                Valid = 1 << 0,

                // Entity handle has been created and finalized.
                Finalized = 1 << 1,

                // Entity handle has been scheduled to be destroyed.
                Destroy = 1 << 2,
            };
        };

        // Handle entry structure.
        struct HandleEntry
        {
            EntityHandle handle;
            int          nextFree;
            uint32_t     flags;
        };

        // Entity command types.
        struct EntityCommands
        {
            enum Type
            {
                Invalid,

                Create,
                Destroy,
            };
        };

        // Entity command structure.
        struct EntityCommand
        {
            EntityCommands::Type type;
            EntityHandle handle;
        };

        // Type declarations.
        typedef std::vector<HandleEntry>  HandleList;
        typedef std::queue<EntityCommand> CommandList;

    private:
        // Frees an entity handle.
        void FreeHandle(int handleIndex, HandleEntry& handleEntry);

    private:
        // List of commands.
        CommandList m_commands;

        // List of entity handles.
        HandleList m_handles;

        // Number of active entities.
        unsigned int m_entityCount;

        // List of free handles.
        int  m_freeListDequeue;
        int  m_freeListEnqueue;
        bool m_freeListIsEmpty;

        // Event dispatchers.
        EventDispatchers m_dispatchers;
    };
}
