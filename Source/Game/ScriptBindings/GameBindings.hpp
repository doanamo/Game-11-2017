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

            // Helper functions.
            Game::EntityHandle* Push(lua_State* state);
            Game::EntityHandle* Push(lua_State* state, const Game::EntityHandle& object);
            Game::EntityHandle* Check(lua_State* state, int index);

            // Metatable methods.
            int New(lua_State* state);
            int Index(lua_State* state);
            int NewIndex(lua_State* state);
        }
    }
}
