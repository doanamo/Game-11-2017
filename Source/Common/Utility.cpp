#include "Precompiled.hpp"
#include "Utility.hpp"

std::string Utility::GetFilePath(std::string filename)
{
    std::string path;

    std::size_t it = filename.find_last_of("/\\");
    if(it != std::string::npos)
    {
        path = filename.substr(0, it + 1);
    }

    return path;
}

std::string Utility::GetFileExtension(std::string filename)
{
    std::string extension;

    std::size_t it = filename.find_last_of(".");
    if(it != std::string::npos)
    {
        extension = filename.substr(it + 1);
    }

    return extension;
}

std::string Utility::GetTextFileContent(std::string filename)
{
    std::ifstream file(filename);
    std::string content;

    if(file)
    {
        file.seekg(0, std::ios::end);
        content.resize((unsigned int)file.tellg());
        file.seekg(0, std::ios::beg);

        file.read(&content[0], content.size());
    }

    return content;
}

std::vector<char> Utility::GetBinaryFileContent(std::string filename)
{
    std::ifstream file(filename, std::ios::binary);
    std::vector<char> content;

    if(file)
    {
        file.seekg(0, std::ios::end);
        content.resize((unsigned int)file.tellg());
        file.seekg(0, std::ios::beg);

        file.read(&content[0], content.size());
    }

    return content;
}

std::vector<std::string> Utility::StringTokenize(std::string text, char character)
{
    std::vector<std::string> result;

    auto begin = text.begin();
    auto it = text.begin();

    while(it != text.end())
    {
        if(*it == character)
        {
            result.push_back(std::string(begin, it));
            begin = ++it;
        }
        else
        {
            ++it;
        }

        if(it == text.end())
        {
            result.push_back(std::string(begin, it));
        }
    }

    return result;
}

std::string Utility::StringLeftTrim(std::string& text, const char* characters)
{
    return text.erase(0, text.find_first_not_of(characters));
}

std::string Utility::StringRightTrim(std::string& text, const char* characters)
{
    return text.erase(text.find_last_not_of(characters) + 1);
}

std::string Utility::StringTrim(std::string& text, const char* characters)
{
    return StringLeftTrim(StringRightTrim(text, characters), characters);
}

std::string Utility::StringReplace(const std::string& source, std::string find, std::string replace)
{
    std::string output = source;
    std::size_t i = 0;

    while((i = output.find(find, i)) != std::string::npos)
    {
        output.replace(i, find.length(), replace);
        i += replace.length();
    }

    return output;
}
