#include "Precompiled.hpp"
#include "Config.hpp"
using namespace System;

namespace
{
    // Log message strings.
    #define LogLoadError(filename) "Could not load a config from \"" << filename << "\" file! "
}

Config::Config()
{
}

Config::~Config()
{
}

void Config::Cleanup()
{
    // Clear the parameter map.
    Utility::ClearContainer(m_parameters);
}

bool Config::LoadFromFile(const std::string filename)
{
    // Open the file.
    std::ifstream file(Build::GetWorkingDir() + filename);

    if(!file.is_open())
    {
        Log() << LogLoadError(filename) << "Could not open the file.";
        return false;
    }

    Log() << "Parsing parameters from \"" << filename << "\" file...";

    // Load parameters from the file.
    std::string section;
    std::string line;

    while(std::getline(file, line))
    {
        if(line.empty())
            continue;

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

            Log() << "Parameter \"" << section << "." << name << "\" has been set to \"" << value << "\".";
        }
    }

    Log() << "Finished loading a config from \"" << filename << "\" file.";

    // Success!
    return true;
}
