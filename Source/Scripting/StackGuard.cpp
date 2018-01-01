#include "Precompiled.hpp"
#include "StackGuard.hpp"
#include "State.hpp"
using namespace Scripting;

StackGuard::StackGuard(lua_State* state) :
    m_state(state),
    m_size(0)
{
    if(m_state != nullptr)
    {
        m_size = lua_gettop(m_state);
    }
}

StackGuard::StackGuard(State& state) :
    StackGuard(state.GetPrivate())
{
}

StackGuard::StackGuard(State* state) :
    StackGuard(state->GetPrivate())
{
}

StackGuard::~StackGuard()
{
    if(m_state != nullptr)
    {
        // Check the current stack size.
        Assert(lua_gettop(m_state) >= m_size, "Stack is smaller than the declared guard size.");

        // Restore the previous stack size.
        lua_settop(m_state, m_size);
    }
}
