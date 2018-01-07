#pragma once

#include "Precompiled.hpp"

// Forward declarations.
namespace Scripting
{
    class State;
}

/*
    Script Bindings
*/

namespace Game
{
    namespace ScriptBindings
    {
        // Register structure that will hold references that will be bound.
        struct References
        {
            References();
        };

        // Registers all script bindings.
        bool Register(Scripting::State* state, const References& references);
    }
}

/*
    Math Bindings
*/

namespace Game
{
    namespace ScriptBindings
    {
        namespace Vec2
        {
            // Registers bindings.
            bool Register(Scripting::State& state);

            // Helper functions.
            glm::vec2* Push(lua_State* state);
            glm::vec2* Push(lua_State* state, const glm::vec2& object);
            glm::vec2* Check(lua_State* state, int index);

            // Metatable methods.
            int New(lua_State* state);
            int Call(lua_State* state);
            int Index(lua_State* state);
            int NewIndex(lua_State* state);

            int Add(lua_State* state);
            int Subtract(lua_State* state);
            int Multiply(lua_State* state);
            int Divide(lua_State* state);
            int Equals(lua_State* state);

            int Length(lua_State* state);
            int LengthSqr(lua_State* state);
            int Truncate(lua_State* state);
            int Normalize(lua_State* state);
        }
    }
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
