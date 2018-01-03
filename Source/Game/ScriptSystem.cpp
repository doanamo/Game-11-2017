#include "Precompiled.hpp"
#include "ScriptSystem.hpp"
#include "ScriptComponent.hpp"
#include "EntitySystem.hpp"
#include "ComponentSystem.hpp"
#include "Scripting/Helpers.hpp"
using namespace Game;

namespace
{
    // Error messages.
    #define LogInitializeError() "Failed to initialize the script system! "
}

ScriptSystemInfo::ScriptSystemInfo() :
    entitySystem(nullptr),
    componentSystem(nullptr),
    garbageCollectionTime(0.002f)
{
}

ScriptSystem::ScriptSystem() :
    m_scriptComponents(nullptr),
    m_initialized(false)
{
    // Bind event receivers.
    m_entityFinalize.Bind<ScriptSystem, &ScriptSystem::FinalizeComponent>(this);
}

ScriptSystem::~ScriptSystem()
{
}

bool ScriptSystem::Initialize(const ScriptSystemInfo& info)
{
    // Check if instance has been already initialized.
    if(m_initialized)
    {
        Log() << LogInitializeError() << "Instance has been already initialized.";
        return false;
    }

    // Validate arguments.
    if(info.entitySystem == nullptr)
    {
        Log() << LogInitializeError() << "Invalid argument - \"info.entitySystem\" is null.";
        return false;
    }

    if(info.componentSystem == nullptr)
    {
        Log() << LogInitializeError() << "Invalid argument - \"info.componentSystem\" is null.";
        return false;
    }

    if(info.garbageCollectionTime <= 0.0f)
    {
        Log() << LogInitializeError() << "Invalid argument - \"info.garbageCollectionTime\" is invalid.";
        return false;
    }

    // Retrieve component pools.
    m_scriptComponents = info.componentSystem->GetPool<Components::Script>();

    SCOPE_GUARD_BEGIN(!m_initialized);
    {
        m_scriptComponents = nullptr;
    }
    SCOPE_GUARD_END();

    if(m_scriptComponents == nullptr)
    {
        Log() << LogInitializeError() << "Could not get the pool for script components.";
        return false;
    }

    // Subscribe to the entity system.
    if(!m_entityFinalize.Subscribe(info.entitySystem->eventDispatchers.entityFinalize))
    {
        Log() << LogInitializeError() << "Could not subscribe to the entity system.";
        return false;
    }

    SCOPE_GUARD_IF(!m_initialized, m_entityFinalize.Unsubscribe());

    // Save initilization parameters.
    m_garbageCollectionTime = info.garbageCollectionTime;

    // Success!
    return m_initialized = true;
}

bool ScriptSystem::FinalizeComponent(EntityHandle entity)
{
    Assert(m_initialized);

    // Finalize a script component.
    auto scriptComponent = m_scriptComponents->Lookup(entity);

    if(scriptComponent != nullptr)
    {
        // Call finalize function on all scripts in a component.
        for(auto& script : scriptComponent->m_scripts)
        {
            /*
            // Setups a stack guard
            Scripting::StackGuard guard(m_state);

            // Push a script instance on the stack.
            Scripting::Push(m_state, script);

            // Call the script finalize method.
            auto result = Scripting::Call<bool>(m_state, "Finalize", Scripting::StackValue(-1), entity);

            if(!result.has_value() || !result.value())
                return false;
            */
        }
    }

    return true;
}

void ScriptSystem::Update(float timeDelta)
{
    if(!m_initialized)
        return;

    // Call update function on all components.
    auto componentsBegin = m_scriptComponents->Begin();
    auto componentsEnd = m_scriptComponents->End();

    for(auto it = componentsBegin; it != componentsEnd; ++it)
    {
        // Get the entity handle.
        const EntityHandle& entity = it->first;

        // Get the script component.
        Components::Script& scriptComponent = it->second;

        // Call update on every script contained in a component.
        for(auto& script : scriptComponent.m_scripts)
        {
            /*
            // Setups a stack guard
            Scripting::StackGuard guard(m_state);

            // Push a script instance on the stack.
            Scripting::Push(m_state, script);

            // Call the script finalize method.
            Scripting::Call(m_state, "Update", Scripting::StackValue(-1), entity, timeDelta);
            */
        }
    }

    // Collect scripting garbage.
    m_state.CollectGarbage(m_garbageCollectionTime);
}

Scripting::State* ScriptSystem::GetState()
{
    return &m_state;
}
