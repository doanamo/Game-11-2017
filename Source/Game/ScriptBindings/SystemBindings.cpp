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

    // Create an userdata pointer.
    void* memory = lua_newuserdata(state, sizeof(System::InputState*));
    auto** pointer = reinterpret_cast<System::InputState**>(memory);
    *pointer = reference;

    // Create a metatable and set it on the userdata pointer.
    luaL_newmetatable(state, "InputState");

    lua_pushliteral(state, "__index");
    lua_pushvalue(state, -2);
    lua_rawset(state, -3);

    lua_pushcfunction(state, ScriptBindings::InputState::IsKeyboardKeyDown);
    lua_setfield(state, -2, "IsKeyboardKeyDown");

    lua_pushcfunction(state, ScriptBindings::InputState::IsKeyboardKeyUp);
    lua_setfield(state, -2, "IsKeyboardKeyUp");

    lua_setmetatable(state, -2);

    // Register as a global variable.
    lua_setglobal(state, "InputState");

    return true;
}

System::InputState* ScriptBindings::InputState::Check(lua_State* state, int index)
{
    Assert(state != nullptr, "Scripting state is nullptr!");

    // Get the userdata object.
    void* memory = luaL_checkudata(state, index, "InputState");
    Assert(memory != nullptr, "Could not get an userdata memory!");

    auto** pointer = reinterpret_cast<System::InputState**>(memory);
    Assert(pointer != nullptr, "Could not get an userdata pointer!");

    return *pointer;
}

int ScriptBindings::InputState::IsKeyboardKeyDown(lua_State* state)
{
    Assert(state != nullptr, "Scripting state is nullptr!");

    // Push an input system reference as the first argument.
    lua_getglobal(state, "InputState");
    lua_insert(state, 1);

    // Get arguments from the stack.
    auto* inputState = InputState::Check(state, 1);
    int   key = (int)luaL_checkinteger(state, 2);
    bool  repeat = luaL_optboolean(state, 3, true);

    // Call the method and push its result.
    bool result = inputState->IsKeyboardKeyDown(key, repeat);
    lua_pushboolean(state, result);

    return 1;
}

int ScriptBindings::InputState::IsKeyboardKeyUp(lua_State* state)
{
    Assert(state != nullptr, "Scripting state is nullptr!");

    // Push an input system reference as the first argument.
    lua_getglobal(state, "InputState");
    lua_insert(state, 1);

    // Get arguments from the stack.
    auto* inputState = InputState::Check(state, 1);
    int   key = (int)luaL_checkinteger(state, 2);
    bool  repeat = luaL_optboolean(state, 3, true);

    // Call the method and push its result.
    bool result = inputState->IsKeyboardKeyUp(key, repeat);
    lua_pushboolean(state, result);

    return 1;
}
