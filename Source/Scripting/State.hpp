#pragma once

#include "Precompiled.hpp"

/*
    Scripting State

    Wraps Lua state object and provides utility.

    void ExampleScriptingState()
    {
        // Create and load a script file.
        Scripting::State scriptingState;
        scriptingState.Load("Data/Scripts/Main.lua");
    }
*/

namespace Scripting
{
    // Scripting state class.
    class State : public NonCopyable
    {
    public:
        State();
        State(lua_State* state);
        State(State&& state);
        State& operator=(State&& other);
        ~State();

        // Creates a scripting state.
        bool Create();

        // Loads a script from a file.
        bool Load(std::string filepath);

        // Parses a script from a string.
        bool Parse(std::string script);

        // Prints an error on the stack and pops it.
        void PrintError();

        // Prints the entire stack for debugging.
        void PrintStack() const;

        // Cleans remaining objects on the stack.
        void CleanStack();

        // Collects all memory garbage.
        void CollectGarbage();

        // Collects memory garbage for a specified time.
        void CollectGarbage(float maxSeconds);

        // Checks if the instance is valid.
        bool IsValid() const;

        // Gets the owner instance of this state.
        State* GetOwner();

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
        lua_State* m_state;
        
        // State owner flag.
        bool m_owner;
    };
}
