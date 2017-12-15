#pragma once

#include "Precompiled.hpp"

/*
    Sprite

    Object that defines a textured quad. Consists of two parts, information
    that can be shared between different instances of sprites and data that
    is unique for each sprite.
*/

namespace Graphics
{
    // Forward declarations.
    class Texture;

    // Sprite structure.
    struct Sprite
    {
        // Info structure shared per batch.
        struct Info
        {
            Info();

            bool operator==(const Info& other) const;
            bool operator!=(const Info& other) const;

            const Texture* texture;
            bool transparent;
            bool filter;
        } info;

        // Data structure of each sprite.
        struct Data
        {
            Data();

            glm::mat4 transform;
            glm::vec4 rectangle;
            glm::vec4 color;
        } data;
    };
}
