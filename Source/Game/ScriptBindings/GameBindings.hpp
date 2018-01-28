#pragma once

#include "Precompiled.hpp"

// Forward declarations.
namespace Scripting
{
    class State;
}

/*
    Entity Handle Bindings
*/

namespace Game
{
    // Forward declarations.
    struct EntityHandle;

    namespace ScriptBindings
    {
        namespace EntityHandle
        {
            // Registers bindings.
            bool Register(Scripting::State& state);

            // Metatable methods.
            int New(lua_State* state);
            int Index(lua_State* state);
            int NewIndex(lua_State* state);
        }
    }
}

/*
    Transform Component Bindings
*/

namespace Game
{
    namespace ScriptBindings
    {
        namespace TransformComponent
        {
            // Registers bindings.
            bool Register(Scripting::State& state);

            // Metatable methods.
            int GetPosition(lua_State* state);
            int SetPosition(lua_State* state);
        }
    }
}
