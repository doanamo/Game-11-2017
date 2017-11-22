#include "Precompiled.hpp"
#include "Buffer.hpp"
using namespace Graphics;

namespace
{
    // Log error messages.
    #define LogCreateError() "Failed to create a buffer! "

    // Constant definitions.
    const GLuint InvalidHandle = 0;
    const GLenum InvalidEnum = 0;
}

BufferInfo::BufferInfo() :
    usage(GL_STATIC_DRAW),
    elementSize(0),
    elementCount(0),
    data(nullptr)
{
}

Buffer::Buffer(GLenum type) :
    m_type(type),
    m_handle(InvalidHandle),
    m_elementSize(0),
    m_elementCount(0)
{
}

Buffer::~Buffer()
{
    this->DestroyHandle();
}

void Buffer::Cleanup()
{
    // Destroy buffer's handle.
    this->DestroyHandle();

    // Reset buffer's parameters.
    m_elementSize = 0;
    m_elementCount = 0;
}

void Buffer::DestroyHandle()
{
    // Release the buffer's handle.
    if(m_handle != InvalidHandle)
    {
        glDeleteBuffers(1, &m_handle);
        m_handle = InvalidHandle;
    }
}

bool Buffer::Create(const BufferInfo& info)
{
    // Check if buffer has been already created.
    if(m_handle != InvalidHandle)
    {
        Log() << LogCreateError() << "Buffer instance has been already initialized.";
        return false;
    }

    // Validate buffer info.
    if(info.elementSize == 0)
    {
        Log() << LogCreateError() << "Invalid argument - \"elementSize\" is 0.";
        return false;
    }

    if(info.elementCount == 0)
    {
        Log() << LogCreateError() << "Invalid argument - \"elementCount\" is 0.";
        return false;
    }

    // Setup a cleanup guard variable.
    bool initialized = false;

    // Create a buffer handle.
    SCOPE_GUARD_IF(!initialized, this->DestroyHandle());

    glGenBuffers(1, &m_handle);

    if(m_handle == InvalidHandle)
    {
        Log() << LogCreateError() << "Could not create a buffer handle.";
        return false;
    }

    // Copy data to the buffer.
    unsigned int bufferSize = info.elementSize * info.elementCount;

    if(info.data != nullptr)
    {
        glBindBuffer(m_type, m_handle);
        glBufferData(m_type, bufferSize, info.data, info.usage);
        glBindBuffer(m_type, 0);
    }

    // Save buffer's parameters.
    m_elementSize = info.elementSize;
    m_elementCount = info.elementCount;

    // Success!
    Log() << "Created " << this->GetName() << " (" << bufferSize << " bytes).";

    return initialized = true;
}

void Buffer::Update(const void* data, int count)
{
    if(m_handle == InvalidHandle)
        return;

    // Validate arguments.
    if(data == nullptr)
        return;

    if(count == 0)
        return;

    // Check if to upload the whole buffer.
    if(count < 0)
    {
        count = m_elementCount;
    }

    // Upload new buffer data.
    glBindBuffer(m_type, m_handle);
    glBufferSubData(m_type, 0, m_elementSize * count, data);
    glBindBuffer(m_type, 0);
}

GLenum IndexBuffer::GetElementType() const
{
    if(m_handle == InvalidHandle)
        return InvalidEnum;

    if(m_type == GL_ELEMENT_ARRAY_BUFFER)
    {
        switch(m_elementSize)
        {
            case 1: return GL_UNSIGNED_BYTE;
            case 2: return GL_UNSIGNED_SHORT;
            case 4: return GL_UNSIGNED_INT;
        }
    }

    return InvalidEnum;
}
