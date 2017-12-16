#pragma once

#include "Precompiled.hpp"

/*
    Lua State

    Manages the internal state of Lua virtual machine.
*/

namespace Scripting
{
    // Scripting state class.
    class State : public NonCopyable
    {
    public:
        State();
        State(State&& state);
        ~State();

        // Initializes the scripting state.
        bool Initialize();

        // Prints an error on the stack and pops it.
        void PrintError();

        // Checks if the instance is valid.
        bool IsValid() const;

        // Gets the private implementation.
        lua_State* GetPrivate();

        // Conversion operator.
        // Allows this class to be used with C functions from Lua API.
        operator lua_State*();

    private:
        // Destroys the internal state.
        void DestroyState();

    private:
        // Virtual machine state.
        lua_State* m_luaState;
    };
}
