#pragma once

#include "Precompiled.hpp"

/*
    System Config

    Simple configuration file format with parameter lookup.

    void ExampleSystemConfig()
    {
        // Create a config instance.
        System::Config config;

        // We can either parse a string or load from a file.
        const char* configText =
            "[Window]\n"
            "Title = \"Game\"\n"
            "Width = 1024\n"
            "Height = 576\n"
            "Vsync = true\n";

        config.Parse(configText);
        config.Load("Game.cfg");
    
        // Retrieve loaded config parameters.
        std::string title = config.GetParameter<std::string>("Window.Title", "Game");
        int width = config.GetParameter<int>("Window.Width", 1024);
        int height = config.GetParameter<int>("Window.Height", 576);
        bool vsync = config.GetParameter<bool>("Window.Vsync", true);
    }
*/

namespace System
{
    // Config class.
    class Config
    {
    public:
        Config();
        ~Config();

        // Reads parameters from a configuration file.
        bool Load(const std::string filename);

        // Parses parameters from a text string.
        bool Parse(const std::string text);
        
        // Sets a config parameter.
        // Can create a parameter that does not exists with provided default value.
        template<typename Type>
        void SetParameter(const std::string name, const Type& value, bool create = true);

        // Gets a config parameter.
        // Returns a specified default value if queried parameter does not exit.
        // Can create a parameter that does not exists with provided default value.
        template<typename Type>
        Type GetParameter(const std::string name, const Type& default, bool create = false);

    private:
        // Converts a value from its type to a string.
        template<typename Type>
        std::string ConvertValueFrom(const Type& value);

        template<>
        std::string ConvertValueFrom(const std::string& value);

        template<>
        std::string ConvertValueFrom(const bool& value);

        // Converts a value from a string back to a type.
        template<typename Type>
        Type ConvertValueTo(const std::string& text);

        template<>
        std::string ConvertValueTo(const std::string& text);

        template<>
        bool ConvertValueTo(const std::string& text);

    private:
        // Type declarations.
        typedef std::map<std::string, std::string> ParameterMap;

    private:
        // List of parameters.
        ParameterMap m_parameters;
    };

    // Template implementations.
    template<typename Type>
    void Config::SetParameter(const std::string name, const Type& value, bool create)
    {
        // Find a parameter by name.
        auto it = m_parameters.find(name);

        if(it == m_parameters.end())
        {
            // Create a new parameter if it does not exist yet.
            if(create)
            {
                auto result = m_parameters.emplace(std::make_pair(name, this->ConvertValueFrom<Type>(value)));
                Assert(result.first != m_parameters.end(), "Failed to insert a new config parameter!");
                Assert(result.second, "Failed to insert a new config parameter!");
            }
        }
        else
        {
            // Assign a new value to the found parameter.
            it->second = this->ConvertValueFrom<Type>(value);
        }
    }

    template<typename Type>
    Type Config::GetParameter(const std::string name, const Type& default, bool create)
    {
        // Find a parameter by name.
        auto it = m_parameters.find(name);

        if(it == m_parameters.end())
        {
            // Create a new parameter if it does not exist yet.
            if(create)
            {
                auto result = m_parameters.emplace(std::make_pair(name, this->ConvertValueFrom<Type>(default)));
                Assert(result.first != m_parameters.end(), "Failed to insert a new config parameter!");
                Assert(result.second, "Failed to insert a new config parameter!");
            }

            // Return the specified default value.
            return default;
        }
        else
        {
            // Convert parameter's value from a string and return it.
            return this->ConvertValueTo<Type>(it->second);
        }
    }

    template<typename Type>
    std::string Config::ConvertValueFrom(const Type& value)
    {
        return std::to_string(value);
    }

    template<>
    std::string Config::ConvertValueFrom(const std::string& value)
    {
        return value;
    }

    template<>
    std::string Config::ConvertValueFrom(const bool& value)
    {
        return value ? "true" : "false";
    }

    template<typename Type>
    Type Config::ConvertValueTo(const std::string& text)
    {
        std::istringstream convert(text);

        Type value;
        convert >> value;
        return value;
    }

    template<>
    std::string Config::ConvertValueTo(const std::string& text)
    {
        return text;
    }

    template<>
    bool Config::ConvertValueTo(const std::string& text)
    {
        return text == "true";
    }
}
