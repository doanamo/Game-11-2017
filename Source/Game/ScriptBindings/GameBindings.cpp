#include "Precompiled.hpp"
#include "GameBindings.hpp"
#include "Scripting/State.hpp"
#include "Scripting/Helpers.hpp"
#include "Game/EntityHandle.hpp"
using namespace Game;

/*
    Entity Handle Bindings
*/

bool ScriptBindings::EntityHandle::Register(Scripting::State& state)
{
    Assert(state.IsValid(), "Invalid scripting state!");

    // Create a class metatable.
    luaL_newmetatable(state, typeid(Game::EntityHandle).name());

    lua_pushcfunction(state, ScriptBindings::EntityHandle::New);
    lua_setfield(state, -2, "New");

    lua_pushcfunction(state, ScriptBindings::EntityHandle::Index);
    lua_setfield(state, -2, "__index");

    lua_pushcfunction(state, ScriptBindings::EntityHandle::NewIndex);
    lua_setfield(state, -2, "__newindex");

    // Create an inner metatable.
    lua_newtable(state);

    lua_pushcfunction(state, ScriptBindings::EntityHandle::New);
    lua_setfield(state, -2, "__call");

    lua_setmetatable(state, -2);

    // Register as a global table.
    lua_setglobal(state, "EntityHandle");

    return true;
}

int ScriptBindings::EntityHandle::New(lua_State* state)
{
    Assert(state != nullptr, "Scripting state is nullptr!");

    // Create a scripting state proxy.
    Scripting::State stateProxy(state);

    // Push a new object.
    Scripting::Push<Game::EntityHandle>(stateProxy);

    return 1;
}

int ScriptBindings::EntityHandle::Index(lua_State* state)
{
    Assert(state != nullptr, "Scripting state is nullptr!");

    // Create a scripting state proxy.
    Scripting::State stateProxy(state);

    // Get arguments from the stack.
    Game::EntityHandle* handle = Scripting::Check<Game::EntityHandle>(stateProxy, 1);
    std::string key = Scripting::Check<std::string>(stateProxy, 2);

    // Return a property.
    if(key == "identifier")
    {
        lua_pushinteger(state, handle->identifier);
        return 1;
    }
    else if(key == "version")
    {
        lua_pushinteger(state, handle->version);
        return 1;
    }
    else
    {
        lua_pushnil(state);
        return 1;
    }
}

int ScriptBindings::EntityHandle::NewIndex(lua_State* state)
{
    Assert(state != nullptr);

    // Create a scripting state proxy.
    Scripting::State stateProxy(state);

    // Get arguments from the stack.
    Game::EntityHandle* handle = Scripting::Check<Game::EntityHandle>(stateProxy, 1);
    std::string key = Scripting::Check<std::string>(stateProxy, 2);

    // Set a property.
    if(key == "identifier")
    {
        handle->identifier = Scripting::Check<Game::EntityHandle::ValueType>(stateProxy, 3);
        return 0;
    }
    else if(key == "version")
    {
        handle->version = Scripting::Check<Game::EntityHandle::ValueType>(stateProxy, 3);
        return 0;
    }

    return 0;
}
