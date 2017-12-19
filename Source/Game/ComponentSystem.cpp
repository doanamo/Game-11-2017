#include "Precompiled.hpp"
#include "ComponentSystem.hpp"
using namespace Game;

ComponentSystem::ComponentSystem()
{
    // Bind event receivers.
    entityFinalize.Bind<ComponentSystem, &ComponentSystem::OnEntityFinalize>(this);
    entityDestroy.Bind<ComponentSystem, &ComponentSystem::OnEntityDestroy>(this);
}

ComponentSystem::~ComponentSystem()
{
}

bool ComponentSystem::OnEntityFinalize(EntityHandle handle)
{
    // Finalize entity components from every pool.
    for(auto& pair : m_pools)
    {
        auto& pool = pair.second;
        return pool->Finalize(handle);
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
