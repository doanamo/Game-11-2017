#include "Precompiled.hpp"
#include "ScriptComponent.hpp"
#include "Scripting/State.hpp"
#include "Scripting/Helpers.hpp"
using namespace Game;
using namespace Components;

namespace
{
    // Error messages.
    #define LogAddScriptError() "Failed to add a new script! "
}

Script::Script()
{
}

Script::~Script()
{
}

bool Script::AddScript(std::shared_ptr<const Scripting::Reference> script)
{
    // Validate script reference.
    if(script == nullptr)
    {
        Log() << LogAddScriptError() << "Invalid argument - \"script\" is null.";
        return false;
    }

    if(!script->IsValid())
    {
        Log() << LogAddScriptError() << "Invalid argument - \"script\" is not valid.";
        return false;
    }

    // Retrieve the scripting state.
    Scripting::State& state = *script->GetState();

    // Create a stack guard.
    Scripting::StackGuard guard(state);

    // Push the table representing the script's type.
    Scripting::Push(state, script);

    // Create a new script instance.
    auto results = Scripting::Call<Scripting::Reference>(state, "New");

    if(!results.has_value())
    {
        Log() << LogAddScriptError() << "Failed to call the instantation function.";
        return false;
    }

    Assert(results.value().IsValid(), "Instantiated script reference is invalid.");

    // Add new script instance to the list.
    m_scripts.push_back(results.value());

    // Success!
    return true;
}
