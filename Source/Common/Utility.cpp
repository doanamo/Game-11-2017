#include "Precompiled.hpp"
#include "Utility.hpp"

std::string Utility::GetFilePath(std::string filepath)
{
    std::string path;

    std::size_t it = filepath.find_last_of("/\\");
    if(it != std::string::npos)
    {
        path = filepath.substr(0, it + 1);
    }

    return path;
}

std::string Utility::GetFileExtension(std::string filepath)
{
    std::string extension;

    std::size_t it = filepath.find_last_of(".");
    if(it != std::string::npos)
    {
        extension = filepath.substr(it + 1);
    }

    return extension;
}

std::string Utility::GetTextFileContent(std::string filepath)
{
    std::ifstream file(filepath);
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

std::vector<char> Utility::GetBinaryFileContent(std::string filepath)
{
    std::ifstream file(filepath, std::ios::binary);
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

std::vector<std::string> Utility::StringTokenize(std::string text, char delimeter)
{
    std::vector<std::string> result;

    auto begin = text.begin();
    auto it = text.begin();

    while(it != text.end())
    {
        if(*it == delimeter)
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

std::string Utility::StringLeftTrim(std::string& text, const char* delimeter)
{
    return text.erase(0, text.find_first_not_of(delimeter));
}

std::string Utility::StringRightTrim(std::string& text, const char* delimeter)
{
    return text.erase(text.find_last_not_of(delimeter) + 1);
}

std::string Utility::StringTrim(std::string& text, const char* delimeter)
{
    return StringLeftTrim(StringRightTrim(text, delimeter), delimeter);
}

std::string Utility::StringReplace(const std::string& source, std::string search, std::string replacement)
{
    std::string output = source;
    std::size_t i = 0;

    while((i = output.find(search, i)) != std::string::npos)
    {
        output.replace(i, search.length(), replacement);
        i += replacement.length();
    }

    return output;
}
