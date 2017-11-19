#include "Precompiled.hpp"
#include "Config.hpp"
using namespace System;

Config::Config() :
    m_initialized(false)
{
}

Config::~Config()
{
    this->Cleanup();
}

void Config::Cleanup()
{
    if(!m_initialized)
        return;

    // Clear the variable map.
    Utility::ClearContainer(m_parameters);

    // Initialization state.
    m_initialized = false;
}

bool Config::Initialize(const std::string filename)
{
    // Cleanup this instance.
    this->Cleanup();

    // Setup a cleanup guard.
    SCOPE_GUARD
    (
        if(!m_initialized)
        {
            m_initialized = true;
            this->Cleanup();
        }
    );

    // Set as already initialized.
    m_initialized = true;

    // Load parameters from a file.
    std::ifstream file(Build::GetWorkingDir() + filename);

    if(file.is_open())
    {
        std::string section;
        std::string line;

        // Parse input file line by line.
        while(std::getline(file, line))
        {
            if(line.empty())
                break;

            if(line.front() == '[' && line.back() == ']')
            {
                // Read the current section.
                section = std::string(line.begin() + 1, line.end() - 1);
            }
            else
            {
                // Find the assignment delimeter.
                std::size_t delimeterPosition = line.find('=');

                if(delimeterPosition == 0 || delimeterPosition == line.length() - 1 || delimeterPosition == std::string::npos)
                {
                    Log() << "Ignored an incorrectly formatted config line: \"" << line << "\".";
                }

                // Read the parameter name and value.
                std::string name = line.substr(0, delimeterPosition);
                std::string value = line.substr(delimeterPosition + 1, std::string::npos);

                // Remove leading and trailing spaces.
                name = Utility::StringTrim(name);
                value = Utility::StringTrim(value);

                // Set a new parameter.
                SetParameter(section + "." + name, value, true);
            }
        }

        Log() << "Loaded a config from \"" << filename << "\" file.";
    }
    else
    {
        Log() << "Could not read a config from \"" << filename << "\" file!";
    }

    // Success!
    return m_initialized;
}
