#include "Precompiled.hpp"
#include "Sampler.hpp"
using namespace Graphics;

namespace
{
    // Log error messages.
    #define LogCreateError() "Failed to create a sampler! "

    // Invalid types.
    const GLuint InvalidHandle = 0;
}

SamplerInfo::SamplerInfo() :
    textureWrapS(GL_REPEAT),
    textureWrapT(GL_REPEAT),
    textureWrapR(GL_REPEAT),
    textureMinFilter(GL_NEAREST_MIPMAP_LINEAR),
    textureMagFilter(GL_LINEAR),
    textureCompareMode(GL_NONE),
    textureCompareFunc(GL_LEQUAL),
    textureMinLOD(-1000.0f),
    textureMaxLOD(1000.0f),
    textureBorderColor(0.0f, 0.0f, 0.0f, 0.0f)
{
}

Sampler::Sampler() :
    m_handle(InvalidHandle)
{
}

Sampler::~Sampler()
{
    this->DestroyHandle();
}

void Sampler::DestroyHandle()
{
    // Release sampler handle.
    if(m_handle != InvalidHandle)
    {
        glDeleteSamplers(1, &m_handle);
        m_handle = InvalidHandle;
    }
}

bool Sampler::Create(const SamplerInfo& info)
{
    // Check if handle has been already created.
    if(m_handle != InvalidHandle)
    {
        Log() << LogCreateError() << "Sampler instance has been already initialized.";
        return false;
    }

    // Setup a cleanup guard.
    bool initialized = false;

    // Create a sampler handle.
    SCOPE_GUARD_IF(!initialized, this->DestroyHandle());

    glGenSamplers(1, &m_handle);

    if(m_handle == InvalidHandle)
    {
        Log() << LogCreateError() << "Could not create a sampler.";
        return false;
    }

    // Set sampling parameters.
    glSamplerParameteri(m_handle, GL_TEXTURE_WRAP_S, info.textureWrapS);
    glSamplerParameteri(m_handle, GL_TEXTURE_WRAP_T, info.textureWrapT);
    glSamplerParameteri(m_handle, GL_TEXTURE_WRAP_R, info.textureWrapR);
    glSamplerParameteri(m_handle, GL_TEXTURE_MIN_FILTER, info.textureMinFilter);
    glSamplerParameteri(m_handle, GL_TEXTURE_MAG_FILTER, info.textureMagFilter);
    glSamplerParameteri(m_handle, GL_TEXTURE_COMPARE_MODE, info.textureCompareMode);
    glSamplerParameteri(m_handle, GL_TEXTURE_COMPARE_FUNC, info.textureCompareFunc);

    glSamplerParameterf(m_handle, GL_TEXTURE_MIN_LOD, info.textureMinLOD);
    glSamplerParameterf(m_handle, GL_TEXTURE_MAX_LOD, info.textureMaxLOD);

    glSamplerParameterfv(m_handle, GL_TEXTURE_BORDER_COLOR, &info.textureBorderColor[0]);

    // Success!
    Log() << "Create a texture sampler.";

    return initialized = true;
}

void Sampler::SetParameter(GLenum parameter, GLint value)
{
    if(m_handle == InvalidHandle)
        return;

    glSamplerParameteri(m_handle, parameter, value);
}

GLuint Sampler::GetHandle() const
{
    return m_handle;
}

bool Sampler::IsValid() const
{
    return m_handle != InvalidHandle;
}
