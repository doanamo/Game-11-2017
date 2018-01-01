#pragma once

#include "Precompiled.hpp"

/*
    Stack Guard

    Scoped guard object that reverts the stack
    to its previous size at the end of its lifetime.

    Example:
        int size = state.GetStackSize();
    
        {
            StackGuard guard(state);
            lua_pushnil(state);
            lua_pushnil(state);
        }
    
        Assert(lua_gettop(state) == 0);
*/

namespace Scripting
{
    // Forward declarations.
    class State;

    // Stack guard class.
    class StackGuard : private NonCopyable
    {
    public:
        StackGuard(lua_State* state);
        StackGuard(State& state);
        StackGuard(State* state);
        ~StackGuard();

    private:
        // Lua state reference.
        lua_State* m_state;

        // Captured stack size.
        int m_size;
    };
}
