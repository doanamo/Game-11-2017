#include "Precompiled.hpp"
#include "Sampler.hpp"
using namespace Graphics;

namespace
{
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
    Log() << "Creating sampler..." << LogIndent();

    // Check if handle has been already created.
    Verify(m_handle == InvalidHandle, "Sampler instance has been already initialized!");

    // Setup a cleanup guard.
    bool initialized = false;

    // Create a sampler handle.
    glGenSamplers(1, &m_handle);

    if(m_handle == InvalidHandle)
    {
        LogError() << "Could not create a sampler!";
        return false;
    }

    SCOPE_GUARD_IF(!initialized, this->DestroyHandle());

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

    Assert(glGetError() == GL_NO_ERROR, "OpenGL error has been encountered!");

    // Success!
    Log() << "Success!";

    return initialized = true;
}

void Sampler::SetParameter(GLenum parameter, GLint value)
{
    Verify(m_handle != InvalidHandle, "Sampler handle has not been created!");

    glSamplerParameteri(m_handle, parameter, value);

    Assert(glGetError() == GL_NO_ERROR, "OpenGL error has been encountered!");
}

GLuint Sampler::GetHandle() const
{
    Verify(m_handle != InvalidHandle, "Sampler handle has not been created!");

    return m_handle;
}

bool Sampler::IsValid() const
{
    return m_handle != InvalidHandle;
}
