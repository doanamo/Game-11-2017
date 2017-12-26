#include "Precompiled.hpp"
#include "ComponentSystem.hpp"
#include "EntitySystem.hpp"
using namespace Game;

ComponentSystem::ComponentSystem()
{
    // Bind event receiver.
    this->eventReceivers.entityDestroy.Bind<ComponentSystem, &ComponentSystem::OnEntityDestroy>(this);
}

ComponentSystem::~ComponentSystem()
{
}

bool ComponentSystem::Subscribe(EntitySystem& entitySystem)
{   
    if(!this->eventReceivers.entityDestroy.Subscribe(entitySystem.eventDispatchers.entityDestroy))
    {
        Log() << "Failed to subscribe a component system! Could not subscribe to dispatchers of an entity system.";
        return false;
    }

    return true;
}

void ComponentSystem::OnEntityDestroy(EntityHandle handle)
{
    // Remove entity components from every pool.
    for(auto& pair : m_pools)
    {
        auto& pool = pair.second;
        pool->Destroy(handle);
    }
}