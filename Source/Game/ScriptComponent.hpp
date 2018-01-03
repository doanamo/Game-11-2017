#pragma once

#include "Precompiled.hpp"
#include "Component.hpp"
#include "Scripting/Reference.hpp"

/*
    Script Component
*/

namespace Game
{
    // Forward declarations.
    class ScriptSystem;

    // Script component class.
    namespace Components
    {
        class Script : public Component
        {
        public:
            // Friend declaration.
            friend ScriptSystem;

        public:
            Script();
            ~Script();

            // Add a script instance.
            bool AddScript(std::shared_ptr<const Scripting::Reference> script);

        private:
            // Type definitions.
            typedef std::vector<Scripting::Reference> ScriptList;

        private:
            // List of scripts.
            ScriptList m_scripts;
        };
    }
}
