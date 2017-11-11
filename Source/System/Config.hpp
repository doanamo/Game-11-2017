#pragma once

#include "Precompiled.hpp"

/*
    Config

    Stores application's configuration which can be
    read from a file and then accessed in runtime.

    Example usage:
        System::Config config;
        config.Initialize("Config.cfg");
    
        width = config.GetVariable<int>("Window.Width", 1024);
        height = config.GetVariable<int>("Window.Height", 576);
        vsync = config.GetVariable<bool>("Window.Vsync", true);
*/

namespace System
{
    // Config class.
    class Config
    {
    public:
        Config();
        ~Config();

        // Restores instance to its original state.
        void Cleanup();

        // Initializes the config.
        // Can load variables from a configuration file.
        bool Initialize(const std::string filename = "");

        // Sets a config variable with provided value.
        template<typename Type>
        void SetVariable(const std::string name, const Type& value);

        // Gets a config variable.
        // Returns a specified default value if queried variable does not exit.
        // Can create a variable that does not exists with provided default value.
        template<typename Type>
        Type GetVariable(const std::string name, const Type& default, bool create = false);

    private:
        // Type declarations.
        typedef std::map<std::string, std::string> VariableMap;

    private:
        // Map of variables.
        VariableMap m_variables;

        // Initialization state.
        bool m_initialized;
    };

    // Template implementations.
    template<typename Type>
    void Config::SetVariable(const std::string name, const Type& value)
    {
        if(!m_initialized)
            return;

        // Set the variable value.
        auto result = m_variables.insert_or_assign(name, std::to_string(value));
        Assert(result.first != m_variables.end(), "Failed to set a config variable!");
    }

    template<typename Type>
    Type Config::GetVariable(const std::string name, const Type& default, bool create)
    {
        if(!m_initialized)
            return Type();

        // Find the variable by name.
        auto it = m_variables.find(name);

        if(it == m_variables.end())
        {
            if(create)
            {
                // Insert a new variable if it does not exist yet.
                auto result = m_variables.insert(std::make_pair(name, std::to_string(default)));
                Assert(result.first != m_variables.end(), "Failed to insert a new config variable!");
                Assert(result.second, "Failed to insert a new config variable!");

                it = result.first;
            }
            else
            {
                // Return default value without inserting a new variable.
                return default;
            }
        }

        // Convert variable's value from a string and return it.
        std::istringstream convert(it->second);

        Type value;
        convert >> value;
        return value;
    }
}
