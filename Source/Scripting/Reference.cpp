#include "Precompiled.hpp"
#include "Reference.hpp"
#include "State.hpp"
using namespace Scripting;

namespace
{
    // Log error messages.
    #define LogLoadError(filename) "Failed to load a script reference from \"" << filename << "\" file! "
}

Reference::Reference() :
    m_state(nullptr),
    m_identifier(LUA_REFNIL)
{
}

Reference::Reference(State* state) :
    m_state(state),
    m_identifier(LUA_REFNIL)
{
}

Reference::Reference(const Reference& other) :
    m_state(other.m_state),
    m_identifier(LUA_REFNIL)
{
    // Create a new reference.
    if(other.IsValid())
    {
        other.PushOntoStack();
        this->CreateFromStack();
    }
}

Reference::Reference(Reference&& other) :
    m_state(std::move(other.m_state)),
    m_identifier(other.m_identifier)
{
    other.m_identifier = LUA_REFNIL;
}

Reference::~Reference()
{
    this->Release();
}

void Reference::Release()
{
    // Release registered reference.
    if(m_identifier != LUA_REFNIL)
    {
        Assert(m_state->IsValid(), "Scripting state is invalid despite having a non-nil reference!");

        luaL_unref(*m_state, LUA_REGISTRYINDEX, m_identifier);
        m_identifier = LUA_REFNIL;
    }
}

bool Reference::Load(std::string filename, State* state)
{
    // Replace reference to scripting state if specified.
    if(state != nullptr)
    {
        m_state = state;
    }

    // Make sure that the scripting state is valid.
    if(m_state == nullptr || !m_state->IsValid())
    {
        Log() << LogLoadError(filename) << "Scripting state is not valid.";
        return false;
    }

    // Check if we are already referencing another object.
    if(m_identifier != LUA_REFNIL)
    {
        Log() << LogLoadError(filename) << "Already referencing another object.";
        return false;
    }

    // Load the script file.
    // Pushes a Lua function on top of the stack.
    if(luaL_loadfile(*m_state, (Build::GetWorkingDir() + filename).c_str()) != 0)
    {
        Log() << LogLoadError(filename) << "Could not load the file.";
        return false;
    }

    // Execute the script.
    // Pushes one result on top of the stack, as specified in arguments.
    if(lua_pcall(*m_state, 0, 1, 0) != 0)
    {
        Log() << LogLoadError(filename) << "Could not execute the script.";
        m_state->PrintError();
        return false;
    }

    // Create a reference from an object on top of the stack.
    // No error checks needed, as we might get a nil which is acceptable.
    m_identifier = luaL_ref(*m_state, LUA_REGISTRYINDEX);

    // Success!
    Log() << "Loaded a script reference from \"" << filename << "\" file.";

    return true;
}

void Reference::CreateFromStack()
{
    if(!this->IsValid())
        return;

    // Create a reference from a stack and pop the object.
    m_identifier = luaL_ref(*m_state, LUA_REGISTRYINDEX);
}

void Reference::PushOntoStack() const
{
    if(!this->IsValid())
        return;

    // Pushes a referenced object on top of the stack.
    lua_rawgeti(*m_state, LUA_REGISTRYINDEX, m_identifier);
}

void Reference::SetState(State* state)
{
    // Release existing reference if there is any.
    this->Release();
    
    // Save a reference to a new state.
    m_state = state;
}

State* Reference::GetState() const
{
    return m_state;
}

Reference::Identifier Reference::GetIdentifier() const
{
    return m_identifier;
}

bool Reference::operator==(const std::nullptr_t) const
{
    return m_identifier == LUA_REFNIL;
}

bool Reference::operator!=(const std::nullptr_t) const
{
    return m_identifier != LUA_REFNIL;
}

bool Reference::IsValid() const
{
    return m_state != nullptr && m_state->IsValid();
}
