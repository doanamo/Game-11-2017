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
    scriptingState(nullptr),
    entitySystem(nullptr),
    componentSystem(nullptr)
{
}

ScriptSystem::ScriptSystem() :
    m_scriptingState(nullptr),
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
    if(info.scriptingState == nullptr)
    {
        Log() << LogInitializeError() << "Invalid argument - \"info.scriptingState\" is null.";
        return false;
    }

    if(!info.scriptingState->IsValid())
    {
        Log() << LogInitializeError() << "Invalid argument - \"info.scriptingState\" is not valid.";
        return false;
    }

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

    // Save reference to the scripting state.
    m_scriptingState = info.scriptingState;

    SCOPE_GUARD_IF(!m_initialized, m_scriptingState = nullptr);

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
            // Setups a stack guard
            Scripting::StackGuard guard(m_scriptingState);

            // Push a script instance on the stack.
            Scripting::Push(*m_scriptingState, script);

            // Call the script finalize method.
            auto result = Scripting::Call<bool>(*m_scriptingState, "Finalize", Scripting::StackValue(-1), entity);

            if(!result.has_value() || !result.value())
                return false;
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
            // Setups a stack guard
            Scripting::StackGuard guard(m_scriptingState);

            // Push a script instance on the stack.
            Scripting::Push(*m_scriptingState, script);

            // Call the script finalize method.
            Scripting::Call(*m_scriptingState, "Update", Scripting::StackValue(-1), entity, timeDelta);
        }
    }
}

Scripting::State* ScriptSystem::GetScriptingState()
{
    return m_scriptingState;
}
