#pragma once

#include "Precompiled.hpp"
#include "StackGuard.hpp"
#include "StackValue.hpp"
#include "StackPopper.hpp"
#include "Reference.hpp"
#include "State.hpp"

/*
    Lua Helpers
*/

inline bool luaL_checkboolean(lua_State* state, int index)
{
    Assert(state != nullptr, "Scripting state is null!");

    if(lua_isboolean(state, index))
    {
        return lua_toboolean(state, index) != 0;
    }
    else
    {
        luaL_error(state, "Not a boolean.");
        return false;
    }
}

inline bool luaL_optboolean(lua_State* state, int index, bool default)
{
    Assert(state != nullptr, "Scripting state is null!");
    return luaL_opt(state, luaL_checkboolean, index, default);
}

/*
    Push()
     
    Pushes a value on top of the stack.
*/

namespace Scripting
{
    template<typename Type, class Enable = void>
    struct PushHelper
    {
        static bool IsNull(const Type& object)
        {
            return false;
        }
    };

    template<typename Type>
    struct PushHelper<Type, typename std::enable_if<std::is_pointer<Type>::value>::type>
    {
        static bool IsNull(const Type& object)
        {
            return object == nullptr;
        }
    };

    inline void Push(State& state)
    {
    }

    // Pushes the global table.
    inline void PushGlobal(State& state)
    {
        Assert(state.IsValid(), "Invalid scripting state!");

        // Push the global table.
        lua_pushglobaltable(state);
    }

    template<typename Type>
    void Push(State& state)
    {
        Assert(state.IsValid(), "Invalid scripting state!");

        // Create a new userdata memory.
        void* memory = lua_newuserdata(state, sizeof(Type));
        Assert(memory != nullptr, "Could not allocate an userdata memory!");

        // Construct an instance in userdata memory.
        auto* instance = new (memory) Type();
        Assert(instance != nullptr, "Could not construct an userdata instance!");

        // Set the object's metatable.
        luaL_getmetatable(state, typeid(std::remove_pointer<Type>::type).name());
        lua_setmetatable(state, -2);
    }

    template<typename Type>
    void Push(State& state, const Type& object)
    {
        Assert(state.IsValid(), "Invalid scripting state!");

        // Check if a pointer is a nullptr (if it is a pointer type).
        if(PushHelper<Type>::IsNull(object))
        {
            lua_pushnil(state);
            return;
        }

        // Create a new userdata memory for the object copy.
        void* memory = lua_newuserdata(state, sizeof(Type));
        Assert(memory != nullptr, "Could not allocate an userdata memory!");

        // Construct a copy instance in userdata memory.
        auto* instance = new (memory) Type(object);
        Assert(instance != nullptr, "Could not construct an userdata instance!");

        // Set the object's metatable.
        luaL_getmetatable(state, typeid(std::remove_pointer<Type>::type).name());
        lua_setmetatable(state, -2);
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
        lua_pushinteger(state, value);
    }

    template<>
    inline void Push(State& state, const long& value)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        lua_pushinteger(state, value);
    }

    template<>
    inline void Push(State& state, const long long& value)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        lua_pushinteger(state, value);
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

    Pops a value from the top of the stack.
*/

namespace Scripting
{
    inline void Pop(State& state, const int count = 1)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        lua_pop(state, count);
    }

    inline void Pop(State& state, const StackValue& value)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        lua_remove(state, value.GetIndex());
    }

    template<typename... Types>
    typename StackPopper<sizeof...(Types), Types...>::ReturnType Pop(State& state)
    {
        return StackPopper<sizeof...(Types), Types...>::Pop(state);
    }
}

/*
    Remove()

    Removes a value from the stack.
*/

namespace Scripting
{
    inline void Remove(State& state, const int index = 1)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        lua_remove(state, index);
    }

    inline void Remove(State& state, const StackValue& value)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        lua_remove(state, value.GetIndex());
    }
}


/*
    Read()

    Reads a value on the stack.
*/

namespace Scripting
{
    template<typename Type>
    struct ReadHelper
    {
        typedef Type* ReturnType;
    };

    template<typename Type>
    typename ReadHelper<Type>::ReturnType Read(State& state, const int index = -1)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        return reinterpret_cast<Type*>(lua_touserdata(state, index));
    }

    template<>
    struct ReadHelper<void>
    {
        typedef void ReturnType;
    };

    template<>
    inline typename ReadHelper<void>::ReturnType Read<void>(State& state, const int index)
    {
    }

    template<>
    struct ReadHelper<std::nullptr_t>
    {
        typedef std::nullptr_t ReturnType;
    };

    template<>
    inline typename ReadHelper<std::nullptr_t>::ReturnType Read<std::nullptr_t>(State& state, const int index)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        return nullptr;
    }

    template<>
    struct ReadHelper<bool>
    {
        typedef bool ReturnType;
    };

    template<>
    inline typename ReadHelper<bool>::ReturnType Read<bool>(State& state, const int index)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        return lua_toboolean(state, index) != 0;
    }

    template<>
    struct ReadHelper<int>
    {
        typedef int ReturnType;
    };

    template<>
    inline typename ReadHelper<int>::ReturnType Read<int>(State& state, const int index)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        return static_cast<int>(lua_tointeger(state, index));
    }

    template<>
    struct ReadHelper<long>
    {
        typedef long ReturnType;
    };

    template<>
    inline typename ReadHelper<long>::ReturnType Read<long>(State& state, const int index)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        return static_cast<long>(lua_tointeger(state, index));
    }

    template<>
    struct ReadHelper<long long>
    {
        typedef long long ReturnType;
    };

    template<>
    inline typename ReadHelper<long long>::ReturnType Read<long long>(State& state, const int index)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        return lua_tointeger(state, index);
    }

    template<>
    struct ReadHelper<float>
    {
        typedef float ReturnType;
    };

    template<>
    inline typename ReadHelper<float>::ReturnType Read<float>(State& state, const int index)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        return static_cast<float>(lua_tonumber(state, index));
    }

    template<>
    struct ReadHelper<double>
    {
        typedef double ReturnType;
    };

    template<>
    inline typename ReadHelper<double>::ReturnType Read<double>(State& state, const int index)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        return lua_tonumber(state, index);
    }

    template<>
    struct ReadHelper<std::string>
    {
        typedef std::string ReturnType;
    };

    template<>
    inline typename ReadHelper<std::string>::ReturnType Read<std::string>(State& state, const int index)
    {
        Assert(state.IsValid(), "Invalid scripting state!");

        if(lua_isnil(state, index))
            return "nil";

        return lua_tostring(state, index);
    }

    template<>
    struct ReadHelper<Reference>
    {
        typedef Reference ReturnType;
    };

    template<>
    inline typename ReadHelper<Reference>::ReturnType Read<Reference>(State& state, const int index)
    {
        Assert(state.IsValid(), "Invalid scripting state!");

        // Clone the value and push it on top of the stack.
        // Creating a reference of the value will pop it, but we want the stack intact.
        lua_pushvalue(state, index);

        // Retrive the original state reference as the provided argument may be a proxy reference.
        State* stateOwner = state.GetOwner();

        // Create a reference to the value at given index.
        Reference reference(stateOwner);
        reference.CreateFromStack();
        return reference;
    }
}

/*
    Check()

    Checks if a value is of a given type a returns its reference (or nullptr if not).
*/

namespace Scripting
{
    template<typename Type>
    struct CheckHelper
    {
        typedef Type* ReturnType;
    };

    template<typename Type>
    typename CheckHelper<Type>::ReturnType Check(State& state, const int index = -1)
    {
        Assert(state.IsValid(), "Invalid scripting state!");

        // Get the userdata object.
        void* memory = luaL_checkudata(state, index, typeid(std::remove_pointer<Type>::type).name());
        Assert(memory != nullptr, "Could not get an userdata memory!");

        // Cast the userdata object.
        Type* instance = reinterpret_cast<Type*>(memory);
        Assert(instance != nullptr, "Could not get an userdata instance!");

        return instance;
    }

    template<>
    struct CheckHelper<bool>
    {
        typedef bool ReturnType;
    };

    template<>
    inline typename CheckHelper<bool>::ReturnType Check<bool>(State& state, const int index)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        return luaL_checkboolean(state, index);
    }

    template<>
    struct CheckHelper<int>
    {
        typedef int ReturnType;
    };

    template<>
    inline typename CheckHelper<int>::ReturnType Check<int>(State& state, const int index)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        return static_cast<int>(luaL_checkinteger(state, index));
    }

    template<>
    struct CheckHelper<long>
    {
        typedef long ReturnType;
    };

    template<>
    inline typename CheckHelper<long>::ReturnType Check<long>(State& state, const int index)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        return static_cast<long>(luaL_checkinteger(state, index));
    }

    template<>
    struct CheckHelper<long long>
    {
        typedef long long ReturnType;
    };

    template<>
    inline typename CheckHelper<long long>::ReturnType Check<long long>(State& state, const int index)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        return static_cast<long long>(luaL_checkinteger(state, index));
    }

    template<>
    struct CheckHelper<float>
    {
        typedef float ReturnType;
    };

    template<>
    inline typename CheckHelper<float>::ReturnType Check<float>(State& state, const int index)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        return static_cast<float>(luaL_checknumber(state, index));
    }

    template<>
    struct CheckHelper<double>
    {
        typedef double ReturnType;
    };

    template<>
    inline typename CheckHelper<double>::ReturnType Check<double>(State& state, const int index)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        return luaL_checknumber(state, index);
    }

    template<>
    struct CheckHelper<std::string>
    {
        typedef std::string ReturnType;
    };

    template<>
    inline typename CheckHelper<std::string>::ReturnType Check<std::string>(State& state, const int index)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        return luaL_checkstring(state, index);
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
        Assert(state.IsValid(), "Invalid scripting state!");
        return luaL_checkudata(state, index, typeid(std::remove_pointer<Type>::type).name()) != nullptr;
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
        return lua_isinteger(state, index) != 0;
    }

    template<>
    inline bool Is<long>(State& state, const int index)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        return lua_isinteger(state, index) != 0;
    }

    template<>
    inline bool Is<long long>(State& state, const int index)
    {
        Assert(state.IsValid(), "Invalid scripting state!");
        return lua_isinteger(state, index) != 0;
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
    // Pushes a nested variable from the global table.
    void GetGlobalField(State& state, std::string name, bool create = false);

    // Pushes a nested variable from the current table.
    void GetField(State& state, std::string name, bool create = false);

    // Pushes a field into the global table.
    template<typename Type>
    void SetGlobalField(State& state, std::string name, Type& object, bool create = false)
    {
        Assert(state.IsValid(), "Invalid scripting state!");

        // Caulcate absolute stack indices for StackValue objects.
        // We have to do this because we will be modifying the stack.
        StackValue::CalculateAbsoluteIndices(state, object);

        // Push the global table.
        Scripting::PushGlobal(state);

        // Set the field in global table.
        Scripting::SetField(state, name, object, create);

        // Remove the global table.
        lua_pop(state, 1);
    }

    // Pushes a field into the current table.
    template<typename Type>
    void SetField(State& state, std::string name, Type& object, bool create = false)
    {
        Assert(state.IsValid(), "Invalid scripting state!");

        // Create a stack guard.
        Scripting::StackGuard guard(state);

        // Caulcate absolute stack indices for StackValue objects.
        // We have to do this because we will be modifying the stack.
        StackValue::CalculateAbsoluteIndices(state, object);

        // Get the field table.
        std::string fieldPath = "";
        std::string fieldName = name;
        std::size_t lastSeparator = name.find_last_of('.');

        if(lastSeparator != std::string::npos)
        {
            // Get field's path and name.
            fieldPath = name.substr(0, lastSeparator);
            fieldName = name.substr(lastSeparator + 1);

            // Push the table from the path.
            Scripting::GetField(state, fieldPath, create);
            if(Scripting::Is<std::nullptr_t>(state, -1))
                return;
        }

        // Push the value on top of the stack.
        Scripting::Push<Type>(state, object);

        // Set value as a field of the table on the stack.
        lua_setfield(state, -2, fieldName.c_str());
    }

    // Calls a function in a table and returns its results as a tuple.
    // Returns an empty optional value if function fails to excute correctly.
    template<typename... Types, typename... Arguments>
    inline std::optional<typename StackPopper<sizeof...(Types), Types...>::ReturnType> Call(State& state, std::string function, const Arguments&... arguments)
    {
        Assert(state.IsValid(), "Invalid scripting state!");

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
