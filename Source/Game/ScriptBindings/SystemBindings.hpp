#pragma once

#include "Precompiled.hpp"

// Forward declarations.
namespace System
{
    class InputState;
}

namespace Scripting
{
    class State;
}

/*
    Input State Bindings
*/

namespace Game
{
    namespace ScriptBindings
    {
        namespace InputState
        {
            // Registers bindings.
            bool Register(Scripting::State& state, System::InputState* reference);

            // Metatable methods.
            int IsKeyboardKeyDown(lua_State* state);
            int IsKeyboardKeyUp(lua_State* state);
        }
    }
}
