#include "Precompiled.hpp"
#include "EntitySystem.hpp"
using namespace Game;

namespace
{
    // Constant variables.
    const int MaximumIdentifier = std::numeric_limits<int>::max();
    const int InvalidIdentifier = 0;
    const int InvalidNextFree = -1;
    const int InvalidQueueElement = -1;
}

EntitySystem::EntitySystem() :
    m_entityCount(0),
    m_freeListDequeue(InvalidQueueElement),
    m_freeListEnqueue(InvalidQueueElement),
    m_freeListIsEmpty(true)
{
}

EntitySystem::~EntitySystem()
{
    // Destroy all remaining entities.
    this->DestroyAllEntities();

    // Empty the processing queue.
    this->ProcessCommands();
}

EntityHandle EntitySystem::CreateEntity()
{
    // Check if we have reached the numerical limits.
    Verify(m_handles.size() != MaximumIdentifier, "Entity identifier limit has been reached!");

    // Create a new handle if the free list queue is empty.
    if(m_freeListIsEmpty)
    {
        // Create an entity handle.
        EntityHandle handle;
        handle.identifier = m_handles.size() + 1;
        handle.version = 0;

        // Create a handle entry.
        HandleEntry entry;
        entry.handle = handle;
        entry.nextFree = InvalidNextFree;
        entry.flags = HandleFlags::Unused;
        m_handles.push_back(entry);

        // Add new handle entry to the free list queue.
        int handleIndex = m_handles.size() - 1;

        m_freeListDequeue = handleIndex;
        m_freeListEnqueue = handleIndex;
        m_freeListIsEmpty = false;
    }

    // Retrieve an unused handle from the free list.
    int handleIndex = m_freeListDequeue;
    HandleEntry& handleEntry = m_handles[handleIndex];

    // Clear next free index on the handle.
    handleEntry.nextFree = InvalidNextFree;

    // Update the free list queue.
    if(m_freeListDequeue == m_freeListEnqueue)
    {
        // If there was only one element in the queue,
        // set the free list queue state to empty.
        m_freeListDequeue = InvalidQueueElement;
        m_freeListEnqueue = InvalidQueueElement;
        m_freeListIsEmpty = true;
    }
    else
    {
        // If there were more than a single element in the queue,
        // set the beginning of the queue to the next free element.
        m_freeListDequeue = handleEntry.nextFree;
    }

    // Mark handle as valid.
    handleEntry.flags |= HandleFlags::Valid;

    // Add a create entity command.
    EntityCommand command;
    command.type = EntityCommands::Create;
    command.handle = handleEntry.handle;
    m_commands.push(command);

    // Return the handle, which is still inactive
    // until the next ProcessCommands() call.
    return handleEntry.handle;
}

void EntitySystem::DestroyEntity(const EntityHandle& entity)
{
    // Check if the handle is valid.
    if(!IsHandleValid(entity))
        return;

    // Locate the handle entry.
    int handleIndex = entity.identifier - 1;
    HandleEntry& handleEntry = m_handles[handleIndex];

    // Set the handle destroy flag.
    handleEntry.flags |= HandleFlags::Destroy;

    // Add a destroy entity command.
    EntityCommand command;
    command.type = EntityCommands::Destroy;
    command.handle = handleEntry.handle;
    m_commands.push(command);
}

void EntitySystem::DestroyAllEntities()
{
    // Process outstanding entity commands.
    ProcessCommands();

    // Check if there are any entities to destroy.
    if(m_handles.empty())
        return;

    // Invalidate all handle entries.
    for(auto it = m_handles.begin(); it != m_handles.end(); ++it)
    {
        HandleEntry& handleEntry = *it;

        if(handleEntry.flags & HandleFlags::Valid)
        {
            // Inform about destroying an entity.
            this->eventDispatchers.entityDestroy(handleEntry.handle);

            // Set the handle flags.
            handleEntry.flags = HandleFlags::Unused;

            // Increment the handle version to invalidate it.
            handleEntry.handle.version += 1;
        }
    }

    // Chain handles to form a free list.
    for(unsigned int i = 0; i < m_handles.size(); ++i)
    {
        HandleEntry& handleEntry = m_handles[i];
        handleEntry.nextFree = i + 1;
    }

    // Close the free list queue chain at the end.
    int lastHandleIndex = m_handles.size() - 1;
    m_handles[lastHandleIndex].nextFree = InvalidNextFree;

    // Set the free list variables.
    m_freeListDequeue = 0;
    m_freeListEnqueue = lastHandleIndex;
    m_freeListIsEmpty = false;
}

void EntitySystem::ProcessCommands()
{
    // Process entity commands.
    while(!m_commands.empty())
    {
        // Get the command from the queue.
        EntityCommand& command = m_commands.front();

        // Process entity command.
        switch(command.type)
        {
            case EntityCommands::Create:
            {
                // Locate the handle entry.
                int handleIndex = command.handle.identifier - 1;
                HandleEntry& handleEntry = m_handles[handleIndex];

                // Make sure handles match.
                Assert(command.handle == handleEntry.handle);

                // Inform that we want this entity finalized.
                if(!this->eventDispatchers.entityFinalize(handleEntry.handle))
                {
                    // Destroy the entity if finalization failed.
                    this->DestroyEntity(handleEntry.handle);
                    break;
                }

                // Mark handle as finalized.
                Assert(!(handleEntry.flags & HandleFlags::Finalized));
                handleEntry.flags |= HandleFlags::Finalized;

                // Increment the counter of active entities.
                m_entityCount += 1;
            }
            break;

        case EntityCommands::Destroy:
        {
            // Locate the handle entry.
            int handleIndex = command.handle.identifier - 1;
            HandleEntry& handleEntry = m_handles[handleIndex];

            // Check if handles match.
            if(command.handle != handleEntry.handle)
            {
                // Trying to destroy an entity twice.
                Assert(false);
                continue;
            }

            // Inform about destroying an entity.
            this->eventDispatchers.entityDestroy(handleEntry.handle);

            // Decrement the counter of active entities.
            m_entityCount -= 1;

            // Free the entity handle and return it to the pool.
            Assert(handleEntry.flags & HandleFlags::Destroy);
            this->FreeHandle(handleIndex, handleEntry);
        }
        break;
        }

        // Remove command from the queue.
        m_commands.pop();
    }
}

void EntitySystem::FreeHandle(int handleIndex, HandleEntry& handleEntry)
{
    // Make sure we got the matching index.
    Assert(&m_handles[handleIndex] == &handleEntry);

    // Make sure that flags are correct.
    Assert(handleEntry.flags & HandleFlags::Valid);
    Assert(!(handleEntry.flags & HandleFlags::Unused));

    // Mark the handle as free.
    handleEntry.flags = HandleFlags::Unused;

    // Increment the handle version to invalidate it.
    handleEntry.handle.version += 1;

    // Add the handle entry to the free list queue.
    if(m_freeListIsEmpty)
    {
        // If there are no elements in the queue,
        // set the element as the only one in the queue.
        m_freeListDequeue = handleIndex;
        m_freeListEnqueue = handleIndex;
        m_freeListIsEmpty = false;
    }
    else
    {
        // Make sure the last element in the queue is valid.
        Assert(m_handles[m_freeListEnqueue].nextFree == InvalidNextFree);

        // If there are other elements in the queue,
        // add the element to the end of the queue chain.
        m_handles[m_freeListEnqueue].nextFree = handleIndex;
        m_freeListEnqueue = handleIndex;
    }
}

bool EntitySystem::IsHandleValid(const EntityHandle& entity) const
{
    // Check if the handle identifier is valid.
    if(entity.identifier <= InvalidIdentifier)
        return false;

    if(entity.identifier > (int)m_handles.size())
        return false;

    // Locate the handle entry.
    int handleIndex = entity.identifier - 1;
    const HandleEntry& handleEntry = m_handles[handleIndex];

    // Check if handle is valid.
    if(!(handleEntry.flags & HandleFlags::Valid))
        return false;

    // Check if handle is scheduled to be destroyed.
    if(handleEntry.flags & HandleFlags::Destroy)
        return false;

    // Check if handle versions match.
    if(handleEntry.handle.version != entity.version)
        return false;

    return true;
}
