#include "Precompiled.hpp"
#include "State.hpp"
using namespace Scripting;

namespace
{
    // Log error messages.
    #define LogCreateError() "Failed to create a scripting state! "
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
    m_state(nullptr)
{
}

State::State(State&& other)
{
    m_state = other.m_state;
    other.m_state = nullptr;
}

State& State::operator=(State&& other)
{
    m_state = other.m_state;
    other.m_state = nullptr;

    return *this;
}

State::~State()
{
    this->DestroyState();
}

void State::DestroyState()
{
    // Cleanup Lua state.
    if(m_state != nullptr)
    {
        lua_close(m_state);
        m_state = nullptr;
    }
}

bool State::Create()
{
    // Check if the instance has been already initialized.
    if(m_state != nullptr)
    {
        Log() << LogCreateError() << "Instance has been already initialized.";
        return false;
    }

    // Setup a cleanup guard variable.
    bool initialized = false;

    // Create Lua state.
    m_state = luaL_newstate();

    if(m_state == nullptr)
    {
        Log() << LogCreateError() << "Could not create a state for Lua virtual machine.";
        return false;
    }

    SCOPE_GUARD_IF(!initialized, this->DestroyState());

    // Load the base library.
    lua_pushcfunction(m_state, luaopen_base);
    lua_pushstring(m_state, "");

    if(lua_pcall(m_state, 1, 0, 0) != 0)
    {
        Log() << LogCreateError() << "Could not load the base library.";
        this->PrintError();
        return false;
    }

    // Register the logging function.
    lua_pushcfunction(m_state, LuaLog);
    lua_setglobal(m_state, "Log");

    // Push a reference to this object.
    lua_pushlightuserdata(m_state, (void*)this);
    lua_setglobal(m_state, "ScriptingState");

    // Make sure that we did not leave anything on the stack.
    Assert(lua_gettop(m_state) == 0, "Stack is not empty.");

    // Success!
    Log() << "Created a scripting Lua state.";

    return initialized = true;
}

bool State::Load(std::string filename)
{
    // Create the state if needed.
    if(m_state == nullptr)
    {
        if(!this->Create())
            return false;
    }

    // Parse the file.
    if(luaL_dofile(m_state, (Build::GetWorkingDir() + filename).c_str()) != 0)
    {
        this->PrintError();
        return false;
    }

    return true;
}

bool State::Parse(std::string text)
{
    // Create the state if needed.
    if(m_state == nullptr)
    {
        if(!this->Create())
            return false;
    }

    // Parse the string.
    if(luaL_dostring(m_state, text.c_str()) != 0)
    {
        this->PrintError();
        return false;
    }

    return true;
}

void State::PrintError()
{
    if(m_state == nullptr)
        return;

    // Make sure that we are getting a string.
    Assert(lua_isstring(m_state, -1), "Expected a string.");

    // Print a string to the log and pop it from the stack.
    Log() << "Lua Error: " << lua_tostring(m_state, -1);
    lua_pop(m_state, 1);
}

void State::PrintStack() const
{
    if(m_state == nullptr)
        return;

    Log() << "Lua stack:";

    // Get the index of the top element.
    int top = lua_gettop(m_state);

    if(top == 0)
    {
        Log() << "  0: Empty";
        return;
    }

    // Print every stack element.
    for(int i = 1; i <= top; ++i)
    {
        int type = lua_type(m_state, i);

        switch(type)
        {
        case LUA_TSTRING:
            Log() << "  " << i << ": \"" << lua_tostring(m_state, i) << "\" (" << lua_typename(m_state, type) << ")";
            break;

        case LUA_TBOOLEAN:
            Log() << "  " << i << ": " << (lua_toboolean(m_state, i) ? "true" : "false") << " (" << lua_typename(m_state, type) << ")";
            break;

        case LUA_TNUMBER:
            Log() << "  " << i << ": " << lua_tonumber(m_state, i) << " (" << lua_typename(m_state, type) << ")";
            break;

        default:
            Log() << "  " << i << ": ??? (" << lua_typename(m_state, type) << ")";
            break;
        }
    }
}

void State::CleanStack()
{
    if(m_state == nullptr)
        return;

    // Discard remaining objects on the Lua stack.
    int size = lua_gettop(m_state);

    if(size != 0)
    {
        Log() << "Cleaning " << size << " abanoned objects on the scripting stack...";
        lua_settop(m_state, 0);
    }
}

void State::CollectGarbage()
{
    if(m_state == nullptr)
        return;

    // Collect all garbage at once.
    lua_gc(m_state, LUA_GCCOLLECT, 0);
}

void State::CollectGarbage(float maxSeconds)
{
    if(m_state == nullptr)
        return;

    if(maxSeconds <= 0.0f)
        return;

    // Run the garbage collector for a specified time.
    double startTime = glfwGetTime();

    do
    {
        if(lua_gc(m_state, LUA_GCSTEP, 0))
            break;
    }
    while((glfwGetTime() - startTime) < maxSeconds);
}

bool State::IsValid() const
{
    return m_state != nullptr;
}

lua_State* State::GetPrivate()
{
    return m_state;
}

State::operator lua_State*()
{
    return m_state;
}
