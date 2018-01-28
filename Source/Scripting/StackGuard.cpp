#include "Precompiled.hpp"
#include "StackGuard.hpp"
#include "State.hpp"
using namespace Scripting;

StackGuard::StackGuard(lua_State* state, bool assert, int removes, int returns) :
    m_state(state), m_assert(assert), m_removes(removes), m_returns(returns)
{
    Assert(state != nullptr, "Scripting state is nullptr!");
    Assert(m_removes <= 0, "Stack element removal count cannot be positive!");
    Assert(m_returns >= 0, "Stack element return count cannot be negative!");

    if(m_state != nullptr)
    {
        m_size = lua_gettop(m_state);
    }
}

StackGuard::StackGuard(State& state, bool assert, int removes, int returns) :
    StackGuard(state.GetPrivate(), assert, removes, returns)
{
    Assert(state.IsValid(), "Invalid scripting state!");
}

StackGuard::StackGuard(State* state, bool assert, int removes, int returns) :
    StackGuard(state->GetPrivate(), assert, removes, returns)
{
    Assert(state->IsValid(), "Invalid scripting state!");
}

StackGuard::~StackGuard()
{
    if(m_state != nullptr)
    {
        // Calculate expected stack size.
        int expectedSize = m_size + m_removes + m_returns;
        int actualSize = lua_gettop(m_state);

        // Check the current stack size.
        if(m_assert)
        {
            Assert(actualSize == expectedSize, "Stack does not match the expected size!");
        }

        // Set the stack to the expected size.
        lua_settop(m_state, expectedSize);
    }
}
