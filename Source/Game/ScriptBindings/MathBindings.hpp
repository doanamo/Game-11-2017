#pragma once

#include "Precompiled.hpp"

// Forward declarations.
namespace Scripting
{
    class State;
}

/*
    Vector 2D Bindings
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
