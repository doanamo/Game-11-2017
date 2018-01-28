#include "Precompiled.hpp"
#include "SystemBindings.hpp"
#include "Scripting/State.hpp"
#include "Scripting/Helpers.hpp"
#include "System/InputState.hpp"
using namespace Game;

/*
    Input State Bindings
*/

bool ScriptBindings::InputState::Register(Scripting::State& state, System::InputState* reference)
{
    Assert(state.IsValid(), "Invalid scripting state!");

    // Create a stack cleanup guard.
    Scripting::StackGuard guard(state);

    // Create a type metatable.
    luaL_newmetatable(state, typeid(System::InputState).name());

    lua_pushliteral(state, "__index");
    lua_pushvalue(state, -2);
    lua_rawset(state, -3);

    lua_pushcfunction(state, ScriptBindings::InputState::IsKeyboardKeyDown);
    lua_setfield(state, -2, "IsKeyboardKeyDown");

    lua_pushcfunction(state, ScriptBindings::InputState::IsKeyboardKeyUp);
    lua_setfield(state, -2, "IsKeyboardKeyUp");

    // Push a reference to input state.
    Scripting::Push<System::InputState*>(state, reference);

    // Register as a global variable.
    Scripting::SetGlobalField(state, "System.InputState", Scripting::StackValue(-1), true);

    return true;
}

int ScriptBindings::InputState::IsKeyboardKeyDown(lua_State* state)
{
    Assert(state != nullptr, "Scripting state is nullptr!");

    // Create a scripting state proxy.
    Scripting::State stateProxy(state);

    // Push an input system reference as the first argument.
    Scripting::GetGlobalField(stateProxy, "System.InputState", false);
    Scripting::Insert(stateProxy, 1);

    // Get arguments from the stack.
    System::InputState* inputState = *Scripting::Check<System::InputState*>(stateProxy, 1);
    int key = Scripting::Check<int>(stateProxy, 2);
    bool repeat = Scripting::Optional<bool>(stateProxy, 3, true);

    // Call the method and push its result.
    Scripting::Push<bool>(stateProxy, inputState->IsKeyboardKeyDown(key, repeat));

    return 1;
}

int ScriptBindings::InputState::IsKeyboardKeyUp(lua_State* state)
{
    Assert(state != nullptr, "Scripting state is nullptr!");

    // Create a scripting state proxy.
    Scripting::State stateProxy(state);

    // Push an input system reference as the first argument.
    Scripting::GetGlobalField(stateProxy, "System.InputState", false);
    Scripting::Insert(stateProxy, 1);

    // Get arguments from the stack.
    System::InputState* inputState = *Scripting::Check<System::InputState*>(stateProxy, 1);
    int key = Scripting::Check<int>(stateProxy, 2);
    bool repeat = Scripting::Optional<bool>(stateProxy, 3, true);

    // Call the method and push its result.
    Scripting::Push<bool>(stateProxy, inputState->IsKeyboardKeyUp(key, repeat));

    return 1;
}
