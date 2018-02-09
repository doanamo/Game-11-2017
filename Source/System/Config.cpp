#include "Precompiled.hpp"
#include "Config.hpp"
using namespace System;

Config::Config()
{
}

Config::~Config()
{
}

bool Config::Load(const std::string filename)
{
    Log() << "Loading config from \"" << filename << "\" file..." << LogIndent();

    // Open the file.
    std::ifstream file(Build::GetWorkingDir() + filename);

    if(!file.is_open())
    {
        LogError() << "Could not open the file!";
        return false;
    }

    // Read the content of the file.
    std::stringstream text;
    text << file.rdbuf();

    // Parse the read config text.
    if(!this->Parse(text.str()))
    {
        LogError() << "Could not parse the file!";
        return false;
    }

    // Success!
    Log() << "Success!";

    return true;
}

bool Config::Parse(const std::string text)
{
    Log() << "Parsing config text..." << LogIndent();

    // Create a string stream.
    std::istringstream stream(text);

    // Load parameters from the file.
    std::string section;
    std::string line;

    while(std::getline(stream, line))
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
                LogWarning() << "Ignoring a config line with ill formatting: \"" << line << "\".";
                break;
            }

            // Read the parameter's name and trim whitespaces.
            std::string name = Utility::StringTrim(line.substr(0, delimeterPosition));

            if(name.empty())
            {
                LogWarning() << "Ignoring a config line with empty parameter name: \"" << line << "\".";
                break;
            }

            // Read the parameter's value and trim whitespaces.
            std::string value = Utility::StringTrim(line.substr(delimeterPosition + 1, std::string::npos));

            if(value.empty())
            {
                LogWarning() << "Ignoring a config line with empty parameter value: \"" << line << "\".";
                break;
            }

            // Remove enclosing quotation marks for string values.
            if(value.front() == '"' && value.back() == '"')
            {
                value = std::string(value.begin() + 1, value.end() - 1);
            }

            // Append current section to the parameter's name.
            if(!section.empty())
            {
                name = section + "." + name;
            }

            // Set a new config parameter.
            this->SetParameter(name, value, true);

            LogInfo() << "Parameter \"" << name << "\" has been set to \"" << value << "\".";
        }
    }

    // Success!
    Log() << "Success!";

    return true;
}
