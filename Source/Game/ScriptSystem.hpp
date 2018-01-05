#pragma once

#include "Precompiled.hpp"
#include "EntityHandle.hpp"
#include "ComponentPool.hpp"
#include "Scripting/State.hpp"

/*
    Script System
*/

namespace Game
{
    // Forward declarations.
    class EntitySystem;
    class ComponentSystem;

    namespace Components
    {
        class Script;
    }

    // Script system info structure.
    struct ScriptSystemInfo
    {
        ScriptSystemInfo();

        Scripting::State* scriptingState;
        EntitySystem* entitySystem;
        ComponentSystem* componentSystem;
    };

    // Script system class.
    class ScriptSystem
    {
    public:
        ScriptSystem();
        ~ScriptSystem();

        // Initializes the scripting system.
        bool Initialize(const ScriptSystemInfo& info);

        // Updates all script components.
        void Update(float timeDelta);

        // Returns the scripting state.
        Scripting::State* GetScriptingState();

    private:
        // Finalizes a script component.
        bool FinalizeComponent(EntityHandle entity);

    private:
        // Scripting state.
        Scripting::State* m_scriptingState;

        // Component pools.
        ComponentPool<Components::Script>* m_scriptComponents;

        // Event receivers.
        Receiver<bool(EntityHandle)> m_entityFinalize;

        // Initialization state.
        bool m_initialized;
    };
}
