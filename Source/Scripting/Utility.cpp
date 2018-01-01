#include "Precompiled.hpp"
#include "Utility.hpp"
using namespace Scripting;

void Scripting::PushGlobal(State& state)
{
    Assert(state.IsValid(), "Invalid scripting state!");
    lua_pushglobaltable(state);
}
