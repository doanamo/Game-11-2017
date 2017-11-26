#pragma once

#include "Precompiled.hpp"

/*
    Sampler
    
    Encapsulates an OpenGL sampler object.
    
    Example usage:
        Graphics::SamplerInfo samplerInfo;
        samplerInfo.textureWrapS = GL_REPEAT;
        samplerInfo.textureWrapR = GL_REPEAT;

        Graphics::Sampler sampler;
        sampler.Create(samplerInfo);
        
        sampler.SetParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        sampler.SetParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        GLuint handle = sampler.GetHandle();
*/

namespace Graphics
{
    // Sampler info structure.
    struct SamplerInfo
    {
        SamplerInfo();

        GLint textureWrapS;
        GLint textureWrapT;
        GLint textureWrapR;
        GLint textureMinFilter;
        GLint textureMagFilter;
        GLint textureCompareMode;
        GLint textureCompareFunc;

        GLfloat textureMinLOD;
        GLfloat textureMaxLOD;

        glm::vec4 textureBorderColor;
    };

    // Sampler class.
    class Sampler
    {
    public:
        Sampler();
        ~Sampler();

        // Initializes the sampler object.
        bool Create(const SamplerInfo& info);

        // Sets a sampler's parameter.
        void SetParameter(GLenum parameter, GLint value);

        // Gets the sampler's handle.
        GLuint GetHandle() const;

        // Checks if the instance valid.
        bool IsValid() const;

    private:
        // Destroys the internal handle.
        void DestroyHandle();

    private:
        // Sampler handle.
        GLuint m_handle;
    };
}
