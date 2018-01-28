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
        StackGuard(lua_State* state, bool assert = false, int removes = 0, int returns = 0);
        StackGuard(State& state, bool assert = false, int removes = 0, int returns = 0);
        StackGuard(State* state, bool assert = false, int removes = 0, int returns = 0);

        ~StackGuard();

    private:
        // Lua state reference.
        lua_State* m_state;

        // Stack modification checks.
        bool m_assert;
        int m_removes;
        int m_returns;

        // Captured stack size.
        int m_size;
    };
}

// Utility macros.
#ifdef NDEBUG
    #define STACK_GUARD(state, removes, returns) ((void)0)
#else
    #define STACK_GUARD(state, removes, returns) Scripting::StackGuard STACK_GUARD_NAME(__LINE__)(state, true, removes, returns)
#endif

#define STACK_GUARD_NAME(line) STACK_GUARD_STRING(line)
#define STACK_GUARD_STRING(line) stackguardLine ## line
