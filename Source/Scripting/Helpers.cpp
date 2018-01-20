#include "Precompiled.hpp"
#include "Helpers.hpp"
using namespace Scripting;

void Scripting::PushGlobal(State& state)
{
    Assert(state.IsValid(), "Invalid scripting state!");
    lua_pushglobaltable(state);
}

void Scripting::PushVariable(State& state, std::string name)
{
    Assert(state.IsValid(), "Invalid scripting state!");

    // Push a copy of the table.
    lua_pushvalue(state, -1);

    // Parse name tokens.
    auto tokens = Utility::TokenizeString(name, '.');

    if(tokens.empty())
    {
        lua_pop(state, -1);
        lua_pushnil(state);
        return;
    }

    // Traverse nested tables.
    for(const std::string& token : tokens)
    {
        // Check if we got a table.
        if(!lua_istable(state, -1))
        {
            lua_pop(state, -1);
            lua_pushnil(state);
            return;
        }

        // Push token key.
        lua_pushstring(state, token.c_str());

        // Get a value from the table.
        lua_gettable(state, -2);

        // Remove the table.
        lua_remove(state, -2);
    }
}
