#pragma once

#include "Precompiled.hpp"

/*
    Utility
*/

namespace Utility
{
    // Gets the size of a static array.
    template<typename Type, size_t Size>
    size_t ArraySize(const Type(&)[Size])
    {
        return Size;
    }

    // Frees container's memory by replacing its content.
    template<typename Type>
    void ClearContainer(Type& container)
    {
        container.swap(Type());
    }

    // Gets the path of a file.
    std::string GetFilePath(std::string filename);

    // Gets the extension of a file.
    std::string GetFileExtension(std::string filename);

    // Gets the content of a text file.
    std::string GetTextFileContent(std::string filename);

    // Gets the content of a binary file.
    std::vector<char> GetBinaryFileContent(std::string filename);

    // Removes leading characters in a string.
    inline std::string LeftStringTrim(std::string& text, const char* characters = " ")
    {
        return text.erase(0, text.find_first_not_of(characters));
    }

    // Removes trailing character in a string.
    inline std::string RightStringTrim(std::string& text, const char* characters = " ")
    {
        return text.erase(text.find_last_not_of(characters) + 1);
    }

    // Removes characters from both ends of a string.
    inline std::string StringTrim(std::string& text, const char* characters = " ")
    {
        return LeftStringTrim(RightStringTrim(text, characters), characters);
    }
}
