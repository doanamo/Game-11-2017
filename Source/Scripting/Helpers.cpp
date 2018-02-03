#include "Precompiled.hpp"
#include "Helpers.hpp"
using namespace Scripting;

void Scripting::GetGlobalField(State& state, std::string name, bool create)
{
    Assert(state.IsValid(), "Invalid scripting state!");

    // Push the global table.
    Scripting::PushGlobal(state);

    // Get the field in global table.
    Scripting::GetField(state, name, create);

    // Remove the global table.
    lua_remove(state, -2);
}

void Scripting::GetField(State& state, std::string name, bool create)
{
    Assert(state.IsValid(), "Invalid scripting state!");

    // Push a copy of the table on the stack.
    lua_pushvalue(state, -1);

    // Parse name tokens.
    auto tokens = Utility::StringTokenize(name, '.');

    if(tokens.empty())
    {
        lua_pop(state, 1);
        lua_pushnil(state);
        return;
    }

    // Traverse nested tables.
    for(const std::string& token : tokens)
    {
        // Check if we got a table.
        if(!lua_istable(state, -1))
        {
            lua_pop(state, 1);
            lua_pushnil(state);
            return;
        }

        // Push token key.
        lua_pushstring(state, token.c_str());

        // Get a value from the table.
        lua_gettable(state, -2);

        // Check if retrived value is nil.
        if(lua_isnil(state, -1) && create)
        {
            // Pop the nil value.
            lua_pop(state, 1);

            // Create a new table.
            lua_newtable(state);

            // Push the new table as a field of an outer table.
            lua_pushvalue(state, -1);
            lua_setfield(state, -3, token.c_str());
        }

        // Remove the table.
        lua_remove(state, -2);
    }
}
