#pragma once

#include "Precompiled.hpp"

/*
    Utility
*/

namespace Utility
{
    // Clamps a value between lower and upper range.
    template<typename Type>
    Type Clamp(const Type& value, const Type& lower, const Type& upper)
    {
        return std::max(lower, std::min(value, upper));
    }

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

    // Splits a string into tokens.
    std::vector<std::string> StringTokenize(std::string text, char character = ' ');

    // Removes leading characters in a string.
    std::string StringLeftTrim(std::string& text, const char* characters = " ");

    // Removes trailing character in a string.
    std::string StringRightTrim(std::string& text, const char* characters = " ");

    // Removes characters from both ends of a string.
    std::string StringTrim(std::string& text, const char* characters = " ");

    // Replace all occurances of text in a string with a replacement text.
    std::string StringReplace(const std::string& source, std::string find, std::string replace);

    // Reorders a vector using given indices.
    template<typename Type>
    void Reorder(std::vector<Type>& values, const std::vector<std::size_t>& order)
    {
        Verify(values.size() == order.size());

        // Check if values are not empty.
        if(values.empty())
            return;

        // Create an array of indices.
        std::vector<std::size_t> indices(order.size());
        std::iota(indices.begin(), indices.end(), 0);

        // Rearange the values.
        for(std::size_t i = 0; i < values.size() - 1; ++i)
        {
            std::size_t desired = order[i];

            for(std::size_t j = i; j < values.size(); ++j)
            {
                if(desired == indices[j])
                {
                    std::swap(values[i], values[j]);
                    std::swap(indices[i], indices[j]);
                    break;
                }
            }
        }
    }
}
