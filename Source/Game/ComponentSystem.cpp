#include "Precompiled.hpp"
#include "ComponentSystem.hpp"
#include "EntitySystem.hpp"
using namespace Game;

ComponentSystem::ComponentSystem()
{
    // Bind event receiver.
    m_entityDestroy.Bind<ComponentSystem, &ComponentSystem::OnEntityDestroy>(this);
}

ComponentSystem::~ComponentSystem()
{
}

bool ComponentSystem::Subscribe(EntitySystem& entitySystem)
{   
    // Receive events about destroyed entities.
    if(!m_entityDestroy.Subscribe(entitySystem.eventDispatchers.entityDestroy))
    {
        Log() << "Failed to subscribe a component system! Could not subscribe to dispatchers of an entity system.";
        return false;
    }

    return true;
}

void ComponentSystem::OnEntityDestroy(EntityHandle handle)
{
    // Remove all components of an entity from every pool.
    for(auto& pair : m_pools)
    {
        auto& pool = pair.second;
        pool->Destroy(handle);
    }
}
