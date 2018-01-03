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

        EntitySystem* entitySystem;
        ComponentSystem* componentSystem;
        float garbageCollectionTime;
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
        Scripting::State* GetState();

    private:
        // Finalizes a script component.
        bool FinalizeComponent(EntityHandle entity);

    private:
        // Event receivers.
        Receiver<bool(EntityHandle)> m_entityFinalize;

        // Component pools.
        ComponentPool<Components::Script>* m_scriptComponents;

        // Scripting state.
        Scripting::State m_state;
        float m_garbageCollectionTime;

        // Initialization state.
        bool m_initialized;
    };
}
