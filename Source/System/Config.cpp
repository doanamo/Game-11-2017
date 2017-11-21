#include "Precompiled.hpp"
#include "Config.hpp"
using namespace System;

namespace
{
    // Log message strings.
    #define LogInitializeError(filename) "Could not load a config from \"" << filename << "\" file! "
}

Config::Config()
{
}

Config::~Config()
{
}

void Config::Cleanup()
{
    // Clear the variable map.
    Utility::ClearContainer(m_parameters);
}

bool Config::LoadFromFile(const std::string filename, bool cleanup)
{
    // Cleanup this instance if requested.
    if(cleanup)
    {
        this->Cleanup();
    }

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
                this->SetParameter(section + "." + name, value, true);
            }
        }

        Log() << "Loaded a config from \"" << filename << "\" file.";
    }
    else
    {
        Log() << LogInitializeError(filename) << "Could not open the file.";
    }

    // Print all read parameters.
    if(!m_parameters.empty())
    {
        Log() << "Printing a list of parameters from \"" << filename << "\" file...";

        for(auto parameter : m_parameters)
        {
            Log() << "Parameter \"" << parameter.first << "\" is set to \"" << parameter.second << "\".";
        }
    }

    return true;
}
