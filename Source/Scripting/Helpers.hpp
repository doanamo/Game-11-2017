#pragma once

#include "Precompiled.hpp"
#include "StackGuard.hpp"
#include "StackValue.hpp"
#include "StackPopper.hpp"
#include "Reference.hpp"
#include "State.hpp"

/*
    Push()
     
    Pushes a value on top of the stack.
*/

namespace Scripting
{
    inline void Push(State& state)
    {
    }

    template<typename Type>
    void Push(State& state, const Type& value)
    {
        static_assert(false, "Not implemented for the given type.");
    }

    template<>
    inline void Push(State& state, const StackValue& value)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        lua_pushvalue(state, value.GetIndex());
    }

    template<>
    inline void Push(State& state, const std::nullptr_t&)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        lua_pushnil(state);
    }

    template<>
    inline void Push(State& state, const bool& value)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        lua_pushboolean(state, value);
    }

    template<>
    inline void Push(State& state, const int& value)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        lua_pushnumber(state, value);
    }

    template<>
    inline void Push(State& state, const float& value)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        lua_pushnumber(state, value);
    }

    template<>
    inline void Push(State& state, const double& value)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        lua_pushnumber(state, value);
    }

    template<>
    inline void Push(State& state, const std::string& value)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        lua_pushstring(state, value.c_str());
    }

    template<>
    inline void Push(State& state, const Reference& reference)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        Assert(&state == reference.GetState(), "Reference is hosted by different scripting state!");
        reference.PushOntoStack();
    }

    template<>
    inline void Push(State& state, const std::shared_ptr<const Reference>& reference)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        Assert(&state == reference->GetState(), "Reference is hosted by different scripting state!");
        reference->PushOntoStack();
    }

    template<size_t Size>
    void Push(State& state, const std::nullptr_t&)
    {
        Assert(state.IsValid(), "Invalid scripting state!");

        for(int i = 0; i < Size; ++i)
        {
            lua_pushnil(state);
        }
    }

    template<size_t Size>
    void Push(State& state, const char(&value)[Size])
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        lua_pushstring(state, value);
    }

    template<typename Type, typename... Types>
    void Push(State& state, const Type& value, const Types&... values)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        Scripting::Push(state, value);
        Scripting::Push(state, values...);
    }
}

/*
    Pop()

    Pops a value from top of the stacks.
*/

namespace Scripting
{
    inline void Pop(State& state, const int count = 1)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        lua_pop(state, count);
    }

    template<typename... Types>
    typename StackPopper<sizeof...(Types), Types...>::ReturnType Pop(State& state)
    {
        return StackPopper<sizeof...(Types), Types...>::Pop(state);
    }
}

/*
    Read()

    Reads a value on the stack.
*/

namespace Scripting
{
    template<typename Type>
    Type Read(State& state, const int index = -1)
    {
        static_assert(false, "Not implemented for the given type.");
    }

    template<>
    inline void Read(State& state, const int index)
    {
    }

    template<>
    inline std::nullptr_t Read(State& state, const int index)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        return nullptr;
    }

    template<>
    inline bool Read(State& state, const int index)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        return lua_toboolean(state, index) != 0;
    }

    template<>
    inline int Read(State& state, const int index)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        return (int)lua_tointeger(state, index);
    }

    template<>
    inline long long Read(State& state, const int index)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        return lua_tointeger(state, index);
    }

    template<>
    inline float Read(State& state, const int index)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        return (float)lua_tonumber(state, index);
    }

    template<>
    inline double Read(State& state, const int index)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        return lua_tonumber(state, index);
    }

    template<>
    inline std::string Read(State& state, const int index)
    {
        Assert(state.IsValid(), "Invalid scripting state!");

        if(lua_isnil(state, index))
            return "nil";

        return lua_tostring(state, index);
    }

    template<>
    inline Reference Read<Reference>(State& state, const int index)
    {
        Assert(state.IsValid(), "Invalid scripting state!");

        // Clone the value and push it on top of the stack.
        // Creating a reference of the value will pop it, but we want the stack intact.
        lua_pushvalue(state, index);

        // Create a reference to the value at given index.
        Reference reference(&state);
        reference.CreateFromStack();
        return reference;
    }
}

/*
    Is()

    Checks if a value is of a given type.
*/

namespace Scripting
{
    template<typename Type>
    bool Is(State& state, const int index = -1)
    {
        static_assert(false, "Not implemented for the given type.");
    }

    template<>
    inline bool Is<std::nullptr_t>(State& state, const int index)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        return lua_isnil(state, index) != 0;
    }

    template<>
    inline bool Is<bool>(State& state, const int index)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        return lua_isboolean(state, index) != 0;
    }

    template<>
    inline bool Is<int>(State& state, const int index)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        return lua_isnumber(state, index) != 0;
    }

    template<>
    inline bool Is<float>(State& state, const int index)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        return lua_isnumber(state, index) != 0;
    }

    template<>
    inline bool Is<double>(State& state, const int index)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        return lua_isnumber(state, index) != 0;
    }

    template<>
    inline bool Is<std::string>(State& state, const int index)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        return lua_isstring(state, index) != 0;
    }
}

/*
    Utility Functions
*/

namespace Scripting
{
    // Pushes the global table.
    void PushGlobal(State& state);

    // Pushes a nested variable from the current table.
    void PushVariable(State& state, std::string name);

    // Calls a function in a table and returns its results as a tuple.
    // Returns an empty optional value if function fails to excute correctly.
    template<typename... Types, typename... Arguments>
    inline std::optional<typename StackPopper<sizeof...(Types), Types...>::ReturnType> Call(State& state, std::string function, const Arguments&... arguments)
    {
        // Create a scope guard.
        StackGuard guard(state);

        // Caulcate absolute stack indices for StackValue objects.
        StackValue::CalculateAbsoluteIndices(state, arguments...);

        // Check if we got a table.
        if(!lua_istable(state, -1))
        {
            // Return an empty optional value.
            return std::nullopt;
        }

        // Get the function from the table.
        lua_getfield(state, -1, function.c_str());

        if(!lua_isfunction(state, -1))
        {
            // Return an empty optional value.
            return std::nullopt;
        }

        // Push function's arguments.
        Push(state, arguments...);

        // Call the function.
        const int returnCount = (int)(sizeof...(Types));
        const int argumentCount = (int)(sizeof...(Arguments));

        if(lua_pcall(state, argumentCount, returnCount, 0) != 0)
        {
            // Print an error and pop it from the stack.
            state.PrintError();

            // Return an empty optional value.
            return std::nullopt;
        }

        // Return function's results.
        return Pop<Types...>(state);
    }
}
