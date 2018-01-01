#pragma once

#include "Precompiled.hpp"

/*
    Scripting Reference

    Saves a reference to an object in Lua state that can be later retrieved.

    Example usage:
        Scripting::Reference reference(&state);
        reference.Load("Scripts/Script.lua");
        reference.PushOntoStack();
*/

namespace Scripting
{
    // Forward declarations.
    class State;

    // Scripting reference class.
    class Reference
    {
    public:
        // Type declarations.
        typedef int Identifier;

    public:
        Reference();
        Reference(State* state);
        Reference(const Reference& other);
        Reference(Reference&& other);
        ~Reference();

        // Release the referenced value.
        void Release();

        // Loads the reference from a file.
        bool Load(std::string filename);

        // Creates a reference for an object on top of the stack.
        // Pops the referenced object from the stack in the process.
        void CreateFromStack();

        // Pushes the referenced value on top of the stack.
        void PushOntoStack() const;

        // Sets the scripting state.
        void SetState(State* state);

        // Gets the scripting state.
        State* GetState() const;

        // Gets the reference identifier.
        Identifier GetIdentifier() const;

        // Checks if the reference is valid.
        bool IsValid() const;

        // Comparison operators.
        bool operator==(const std::nullptr_t) const;
        bool operator!=(const std::nullptr_t) const;

    private:
        // Scripting state.
        State* m_state;

        // Reference indentifier.
        Identifier m_identifier;
    };
}
