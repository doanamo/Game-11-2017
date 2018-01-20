#include "Precompiled.hpp"
#include "GameBindings.hpp"
#include "Scripting/State.hpp"
#include "Game/EntityHandle.hpp"
using namespace Game;

/*
    Entity Handle Bindings
*/

bool ScriptBindings::EntityHandle::Register(Scripting::State& state)
{
    Assert(state.IsValid(), "Invalid scripting state!");

    // Create a class metatable.
    luaL_newmetatable(state, "EntityHandle");

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

Game::EntityHandle* ScriptBindings::EntityHandle::Push(lua_State* state)
{
    Assert(state != nullptr, "Scripting state is nullptr!");

    // Create a new userdata object.
    void* memory = lua_newuserdata(state, sizeof(Game::EntityHandle));
    Assert(memory != nullptr, "Could not allocate an userdata memory!");

    auto* instance = new (memory) Game::EntityHandle();
    Assert(instance != nullptr, "Could not construct an userdata instance!");

    // Set the object's metatable.
    luaL_getmetatable(state, "EntityHandle");
    lua_setmetatable(state, -2);

    return instance;
}

Game::EntityHandle* ScriptBindings::EntityHandle::Push(lua_State* state, const Game::EntityHandle& object)
{
    Assert(state != nullptr, "Scripting state is nullptr!");

    // Create an userdata for the object copy.
    void* memory = lua_newuserdata(state, sizeof(Game::EntityHandle));
    Assert(memory != nullptr, "Could not allocate an userdata memory!");

    auto* instance = new (memory) Game::EntityHandle(object);
    Assert(instance != nullptr, "Could not construct an userdata instance!");

    // Set the object's metatable.
    luaL_getmetatable(state, "EntityHandle");
    lua_setmetatable(state, -2);

    return instance;
}

Game::EntityHandle* ScriptBindings::EntityHandle::Check(lua_State* state, int index)
{
    Assert(state != nullptr, "Scripting state is nullptr!");

    // Get the userdata object.
    void* memory = luaL_checkudata(state, index, "EntityHandle");
    Assert(memory != nullptr, "Could not get an userdata memory!");

    auto* instance = reinterpret_cast<Game::EntityHandle*>(memory);
    Assert(instance != nullptr, "Could not get an userdata instance!");

    return instance;
}

int ScriptBindings::EntityHandle::New(lua_State* state)
{
    Assert(state != nullptr, "Scripting state is nullptr!");

    // Push a new object.
    ScriptBindings::EntityHandle::Push(state);

    return 1;
}

int ScriptBindings::EntityHandle::Index(lua_State* state)
{
    Assert(state != nullptr, "Scripting state is nullptr!");

    // Get arguments from the stack.
    auto* handle = ScriptBindings::EntityHandle::Check(state, 1);
    std::string key = luaL_checkstring(state, 2);

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

    // Get arguments from the stack.
    auto* handle = ScriptBindings::EntityHandle::Check(state, 1);
    std::string key = luaL_checkstring(state, 2);

    // Set a property.
    if(key == "identifier")
    {
        handle->identifier = (Game::EntityHandle::ValueType)luaL_checkinteger(state, 3);
        return 0;
    }
    else if(key == "version")
    {
        handle->version = (Game::EntityHandle::ValueType)luaL_checkinteger(state, 3);
        return 0;
    }

    return 0;
}
