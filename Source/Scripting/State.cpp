#include "Precompiled.hpp"
#include "State.hpp"
using namespace Scripting;

namespace
{
    // Log error messages.
    #define LogInitializeError() "Failed to initialize a scripting state! "
}

extern "C"
{
    // Prints a string on the stack to the log stream.
    static int LuaLog(lua_State* luaState)
    {
        Assert(luaState != nullptr, "Lua state is invalid.");

        if(lua_isstring(luaState, -1))
        {
            Log() << lua_tostring(luaState, -1);
        }

        return 0;
    }
}

State::State() :
    m_luaState(nullptr)
{
}

State::State(State&& other)
{
    m_luaState = other.m_luaState;
    other.m_luaState = nullptr;
}

State::~State()
{
    this->DestroyState();
}

void State::DestroyState()
{
    // Cleanup Lua state.
    if(m_luaState != nullptr)
    {
        lua_close(m_luaState);
        m_luaState = nullptr;
    }
}

bool State::Initialize()
{
    // Check if the instance has been already initialized.
    if(m_luaState != nullptr)
    {
        Log() << LogInitializeError() << "Instance has been already initialized.";
        return false;
    }

    // Setup a cleanup guard variable.
    bool initialized = false;

    // Create Lua state.
    m_luaState = luaL_newstate();

    if(m_luaState == nullptr)
    {
        Log() << LogInitializeError() << "Could not create a state for Lua virtual machine.";
        return false;
    }

    SCOPE_GUARD_IF(!initialized, this->DestroyState());

    // Load the base library.
    lua_pushcfunction(m_luaState, luaopen_base);
    lua_pushstring(m_luaState, "");

    if(lua_pcall(m_luaState, 1, 0, 0) != 0)
    {
        Log() << LogInitializeError() << "Could not load the base library.";
        this->PrintError();
        return false;
    }

    // Register the logging function.
    lua_pushcfunction(m_luaState, LuaLog);
    lua_setglobal(m_luaState, "Log");

    // Make sure that we did not leave anything on the stack.
    Assert(lua_gettop(m_luaState) == 0, "Stack is not empty.");

    // Success!
    Log() << "Created a scripting Lua state.";

    return initialized = true;
}

bool State::Load(std::string filename)
{
    // Initialize if needed.
    if(m_luaState == nullptr)
    {
        if(!this->Initialize())
            return false;
    }

    // Parse the file.
    if(luaL_dofile(m_luaState, (Build::GetWorkingDir() + filename).c_str()) != 0)
    {
        this->PrintError();
        return false;
    }

    return true;
}

bool State::Parse(std::string text)
{
    // Initialize if needed.
    if(m_luaState == nullptr)
    {
        if(!this->Initialize())
            return false;
    }

    // Parse the string.
    if(luaL_dostring(m_luaState, text.c_str()) != 0)
    {
        this->PrintError();
        return false;
    }

    return true;
}

void State::PrintError()
{
    if(m_luaState == nullptr)
        return;

    // Make sure that we are getting a string.
    Assert(lua_isstring(m_luaState, -1), "Expected a string.");

    // Print a string to the log and pop it from the stack.
    Log() << "Lua Error: " << lua_tostring(m_luaState, -1);
    lua_pop(m_luaState, 1);
}

bool State::IsValid() const
{
    return m_luaState != nullptr;
}

lua_State* State::GetPrivate()
{
    return m_luaState;
}

State::operator lua_State*()
{
    return m_luaState;
}
