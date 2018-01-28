#include "Precompiled.hpp"
#include "ScriptBindings.hpp"
#include "Scripting/State.hpp"
#include "ScriptBindings/MathBindings.hpp"
#include "ScriptBindings/InputBindings.hpp"
#include "ScriptBindings/SystemBindings.hpp"
#include "ScriptBindings/GameBindings.hpp"
using namespace Game;

namespace
{
    // Error messages.
    #define LogRegisterError() "Failed to register script bindings! "
}

/*
    Script Bindings
*/

ScriptBindings::References::References() :
    inputState(nullptr)
{
}

bool ScriptBindings::Register(Scripting::State* state, const References& references)
{
    // Validate arguments.
    if(state == nullptr)
    {
        Log() << LogRegisterError() << "Invalid argument - \"state\" is null.";
        return false;
    }

    if(!state->IsValid())
    {
        Log() << LogRegisterError() << "Invalid argument - \"state\" is not valid.";
        return false;
    }

    // Register bindings.
    bool results = true;

    results &= ScriptBindings::Vec2::Register(*state);
    results &= ScriptBindings::Vec3::Register(*state);
    results &= ScriptBindings::KeyboardKeys::Register(*state);
    results &= ScriptBindings::InputState::Register(*state, references.inputState);
    results &= ScriptBindings::EntityHandle::Register(*state);

    return results;
}
