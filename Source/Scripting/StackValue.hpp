#pragma once

#include "Precompiled.hpp"
#include "State.hpp"

/*
    Scripting Stack Value

    Objects that references another object already on the stack by its index.
    This allows for example to push the table with a function as the function's
    argument, assuming that we know its exact relative index on the current stack.

    void ExampleScriptingStackValue(Scripting::State& state, Scripting::Reference& script)
    {
        // Push an object on top of the stack.
        Scripting::Push(state, scriptObject);

        // Call a method of a pushed script object.
        Scripting::Call(state, "Update", Scripting::StackValue(-1));
    }
*/

namespace Scripting
{
    // Stack value class.
    struct StackValue
    {
    public:
        // Consturctor.
        StackValue(int index);

        // Converts relative indices of provided values on the stack to absolute indices.
        // Declarations of following template functions matters, so do not reorder them!
        static void CalculateAbsoluteIndices(State& state)
        {
            // Do not do anything for no arguments.
        }

        template<typename Type>
        static void CalculateAbsoluteIndices(State& state, const Type& value)
        {
            // Do not do anything for almost all types.
        }

        template<>
        static void CalculateAbsoluteIndices(State& state, const StackValue& value)
        {
            Verify(state.IsValid(), "Scripting state is invalid!");

            // Convert relative index to an absolute position.
            if(value.m_index < 0)
            {
                value.m_index = lua_gettop(state) + (value.m_index + 1);
            }
        }

        template<typename Type, typename... Types>
        static void CalculateAbsoluteIndices(State& state, const Type& value, const Types&... values)
        {
            Verify(state.IsValid(), "Scripting state is invalid!");

            // Call the template function on all values.
            CalculateAbsoluteIndices(state, value);
            CalculateAbsoluteIndices(state, values...);
        }

        // Returns the referenced index.
        int GetIndex() const;

    private:
        // Relative or absolute index on the stack.
        // Relative indices can only have negative values.
        // Mutable state allows it to be converted from relative to absolute index.
        mutable int m_index;
    };
}
