#pragma once

#include "Precompiled.hpp"

/*
    Texture
    
    Encapsulates an OpenGL texture surface.
    Can also load images from PNG files.
    
    Example usage:
        Graphics::Texture texture;
        texture.Load("image.png");
        
        GLuint handle = texture.GetHandle();
*/

namespace Graphics
{
    // Texture class.
    class Texture
    {
    public:
        Texture();
        ~Texture();

        // Loads the texture from a file.
        bool Load(std::string filename);

        // Initializes the texture instance.
        bool Create(int width, int height, GLenum format, const void* data);

        // Updates the texture data.
        void Update(const void* data);

        // Gets the texture handle.
        GLuint GetHandle() const
        {
            return m_handle;
        }

        // Gets the texture width.
        int GetWidth() const
        {
            return m_width;
        }

        // Gets the texture height.
        int GetHeight() const
        {
            return m_height;
        }

        // Checks if instance is valid.
        bool IsValid() const;

    private:
        // Destroys the internal handle.
        void DestroyHandle();

    private:
        // Texture handle.
        GLuint m_handle;

        // Texture parameters.
        int m_width;
        int m_height;
        GLenum m_format;
    };
}
