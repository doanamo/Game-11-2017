#include "Precompiled.hpp"
#include "State.hpp"
using namespace Scripting;

extern "C"
{
    // Prints a string on the stack to the log stream.
    static int LuaLog(lua_State* luaState)
    {
        Assert(luaState != nullptr, "Lua state is invalid.");

        if(lua_isstring(luaState, -1))
        {
            LogInfo() << lua_tostring(luaState, -1);
        }

        return 0;
    }
}

State::State() :
    m_state(nullptr),
    m_owner(false)
{
}

State::State(lua_State* state) :
    m_state(state),
    m_owner(false)
{
}

State::State(State&& other)
{
    m_state = other.m_state;
    other.m_state = nullptr;

    m_owner = other.m_owner;
    other.m_owner = false;
}

State& State::operator=(State&& other)
{
    this->DestroyState();

    m_state = other.m_state;
    other.m_state = nullptr;

    m_owner = other.m_owner;
    other.m_owner = false;

    return *this;
}

State::~State()
{
    this->DestroyState();
}

void State::DestroyState()
{
    // Cleanup Lua state.
    if(m_owner)
    {
        if(m_state != nullptr)
        {
            lua_close(m_state);
            m_state = nullptr;
        }
    }
}

bool State::Create()
{
    Log() << "Creating scripting state..." << LogIndent();

    // Check if the instance has been already initialized.
    Verify(m_state == nullptr, "Instance has been already initialized!");

    // Setup a cleanup guard variable.
    bool initialized = false;

    // Create Lua state.
    m_state = luaL_newstate();

    if(m_state == nullptr)
    {
        LogError() << "Could not create a new Lua state!";
        return false;
    }

    SCOPE_GUARD_IF(!initialized, this->DestroyState());

    // Load the base library.
    lua_pushcfunction(m_state, luaopen_base);
    lua_pushstring(m_state, "");

    if(lua_pcall(m_state, 1, 0, 0) != 0)
    {
        LogError() << "Could not load the base library!";
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

    // Set this instance as the owner of the created state.
    m_owner = true;

    // Success!
    LogInfo() << "Success!";

    return initialized = true;
}

bool State::Load(std::string filepath)
{
    Log() << "Loading scripting state from \"" << filepath << "\" file..." << LogIndent();

    // Create the state if needed.
    if(m_state == nullptr)
    {
        if(!this->Create())
            return false;
    }

    // Parse the file.
    if(luaL_dofile(m_state, (Build::GetWorkingDir() + filepath).c_str()) != 0)
    {
        LogError() << "Could not load the file!";
        this->PrintError();
        return false;
    }

    // Success!
    LogInfo() << "Success!";

    return true;
}

bool State::Parse(std::string text)
{
    Log() << "Parsing scripting state from string...";

    // Create the state if needed.
    if(m_state == nullptr)
    {
        if(!this->Create())
            return false;
    }

    // Parse the string.
    if(luaL_dostring(m_state, text.c_str()) != 0)
    {
        LogError() << "Could not load the string!";
        this->PrintError();
        return false;
    }

    // Success!
    LogInfo() << "Success!";

    return true;
}

void State::PrintError()
{
    Verify(m_state != nullptr, "Scripting state instance is not initialized!");

    // Make sure that we are getting a string.
    Assert(lua_isstring(m_state, -1), "Expected a string.");

    // Print a string to the log and pop it from the stack.
    LogWarning() << "Lua Error: " << lua_tostring(m_state, -1);
    lua_pop(m_state, 1);
}

void State::PrintStack() const
{
    Verify(m_state != nullptr, "Scripting state instance is not initialized!");

    // Starting printing the stack.
    LogInfo() << "Lua stack:";

    // Get the index of the top element.
    int top = lua_gettop(m_state);

    if(top == 0)
    {
        LogInfo() << "  0: Empty";
        return;
    }

    // Print every stack element.
    for(int i = 1; i <= top; ++i)
    {
        int type = lua_type(m_state, i);

        switch(type)
        {
        case LUA_TSTRING:
            LogInfo() << "  " << i << ": \"" << lua_tostring(m_state, i) << "\" (" << lua_typename(m_state, type) << ")";
            break;

        case LUA_TBOOLEAN:
            LogInfo() << "  " << i << ": " << (lua_toboolean(m_state, i) ? "true" : "false") << " (" << lua_typename(m_state, type) << ")";
            break;

        case LUA_TNUMBER:
            LogInfo() << "  " << i << ": " << lua_tonumber(m_state, i) << " (" << lua_typename(m_state, type) << ")";
            break;

        default:
            LogInfo() << "  " << i << ": ??? (" << lua_typename(m_state, type) << ")";
            break;
        }
    }
}

void State::CleanStack()
{
    Verify(m_state != nullptr, "Scripting state instance is not initialized!");

    // Discard remaining objects on the Lua stack.
    int size = lua_gettop(m_state);

    if(size != 0)
    {
        LogDebug() << "Cleaning " << size << " abandoned objects on the scripting stack...";
        lua_settop(m_state, 0);
    }
}

void State::CollectGarbage()
{
    Verify(m_state != nullptr, "Scripting state instance is not initialized!");

    // Collect all garbage at once.
    lua_gc(m_state, LUA_GCCOLLECT, 0);
}

void State::CollectGarbage(float maxSeconds)
{
    Verify(m_state != nullptr, "Scripting state instance is not initialized!");
    Verify(maxSeconds > 0.0f, "Invalid argument - \"maxSeconds\" is invalid!");

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

State* State::GetOwner()
{
    Verify(m_state != nullptr, "Scripting state instance is not initialized!");

    // Retrieve pointer to the instance that owns this Lua state.
    lua_getglobal(m_state, "ScriptingState");
    
    // Get a pointer to the owner.
    State* owner = (State*)lua_touserdata(m_state, -1);
    lua_pop(m_state, 1);

    // Return the pointer.
    return owner;
}

lua_State* State::GetPrivate()
{
    Verify(m_state != nullptr, "Scripting state instance is not initialized!");

    return m_state;
}

State::operator lua_State*()
{
    Verify(m_state != nullptr, "Scripting state instance is not initialized!");

    return m_state;
}
