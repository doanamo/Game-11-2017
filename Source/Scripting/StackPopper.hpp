#pragma once

#include "Precompiled.hpp"
#include "State.hpp"

/*
    Stack Popper
    
    Pops multiple values from the stacks and returns them as a tuple.
*/

namespace Scripting
{
    // Stack popper class for multipled returned objects.
    template<size_t, typename... Types>
    struct StackPopper
    {
    public:
        // Type declarations.
        typedef std::tuple<Types...> ReturnType;

    public:
        // Pops multiple objects from the stack and returns them in a tuple.
        static ReturnType Pop(State& state)
        {
            Assert(state.IsValid(), "Invalid scripting state!");
            auto value = CreateTuple<Types...>(state, -(int)(sizeof...(Types)));
            lua_pop(state, (int)(sizeof...(Types)));
            return value;
        }

        // Recursive function for creating a tuple.
        template<typename Type>
        static std::tuple<Type> CreateTuple(State& state, const int index)
        {
            Assert(state.IsValid(), "Invalid scripting state!");
            return std::make_tuple(Read<Type>(state, index));
        }

        template<typename Type1, typename Type2, typename... Rest>
        static std::tuple<Type1, Type2, Rest...> CreateTuple(State& state, const int index)
        {
            Assert(state.IsValid(), "Invalid scripting state!");
            std::tuple<Type1> head = std::make_tuple(Read<Type1>(state, index));
            return std::tuple_cat(head, CreateTuple<Type2, Rest...>(state, index + 1));
        }
    };

    // Stack popper class for one returned object.
    template<typename Type>
    struct StackPopper<1, Type>
    {
    public:
        // Type declarations.
        typedef Type ReturnType;

    public:
        // Pops an object from the stack and returns it.
        static ReturnType Pop(State& state)
        {
            Assert(state.IsValid(), "Invalid scripting state!");
            ReturnType value = Read<Type>(state, -1);
            lua_pop(state, 1);
            return value;
        }
    };

    // Stack popper class for no returned objects.
    template<typename... Types>
    struct StackPopper<0, Types...>
    {
    public:
        // Type declarations.
        // This used to be of type void, but caused issues with templates.
        typedef std::nullptr_t ReturnType;

    public:
        // Pops nothing from the stack and return nothing.
        static ReturnType Pop(State& state)
        {
            return nullptr;
        }
    };
}
