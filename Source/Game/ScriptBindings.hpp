#pragma once

#include "Precompiled.hpp"

// Forward declarations.
namespace Scripting
{
    class State;
}

namespace System
{
    class InputState;
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

            System::InputState* inputState;
        };

        // Registers all script bindings.
        bool Register(Scripting::State* state, const References& references);
    }
}
