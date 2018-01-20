#pragma once

#include "Precompiled.hpp"

// Forward declarations.
namespace Scripting
{
    class State;
}

/*
    Keyboard Bindings
*/

namespace Game
{
    namespace ScriptBindings
    {
        namespace KeyboardKeys
        {
            // Registers bindings.
            bool Register(Scripting::State& state);
        }
    }
}
