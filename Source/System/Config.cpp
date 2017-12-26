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

bool Config::Load(const std::string filename)
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
        // Check if the line is empty.
        if(line.empty())
            continue;

        // Check if the line contains a section specifier.
        if(line.front() == '[' && line.back() == ']')
        {
            // Read the current section.
            section = std::string(line.begin() + 1, line.end() - 1);

            // Move to the next line.
            continue;
        }

        // Parse a config parameter.
        {
            // Find the assignment delimeter.
            std::size_t delimeterPosition = line.find('=');

            // Check if the assignment delimeter has a correct position.
            bool incorrectlyFormatted = false;

            incorrectlyFormatted |= delimeterPosition == 0;                 // Delimeter cannot be the first character.
            incorrectlyFormatted |= delimeterPosition == line.length() - 1; // Delimeter cannot be the last character.
            incorrectlyFormatted |= delimeterPosition == std::string::npos; // Delimeter must be present.

            if(incorrectlyFormatted)
            {
                Log() << "Ignoring a config line with incorrectly formatted delimeter: \"" << line << "\".";
                break;
            }

            // Read the parameter's name and trim whitespaces.
            std::string name = Utility::StringTrim(line.substr(0, delimeterPosition));

            if(name.empty())
            {
                Log() << "Ignoring a config line with an empty parameter name: \"" << line << "\".";
                break;
            }

            // Read the parameter's value and trim whitespaces.
            std::string value = Utility::StringTrim(line.substr(delimeterPosition + 1, std::string::npos));

            if(value.empty())
            {
                Log() << "Ignoring a config line with an empty parameter value: \"" << line << "\".";
                break;
            }

            // Append current section to the parameter's name.
            if(!section.empty())
            {
                name = section + "." + name;
            }

            // Set a new config parameter.
            this->SetParameter(name, value, true);

            Log() << "Parsed parameter \"" << name << "\" has been set to \"" << value << "\".";
        }
    }

    Log() << "Finished parsing a config from \"" << filename << "\" file.";

    // Success!
    return true;
}
