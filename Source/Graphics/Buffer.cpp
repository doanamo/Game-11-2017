#include "Precompiled.hpp"
#include "Buffer.hpp"
using namespace Graphics;

namespace
{
    // Constant definitions.
    const GLuint InvalidHandle = 0;
    const GLenum InvalidEnum = 0;
}

/*
    Buffer
*/

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
    Log() << "Creating " << this->GetName() << "..." << LogIndent();

    // Check if the handle has been already created.
    Verify(m_handle == InvalidHandle, "Buffer instance has been already initialized!");

    // Validate buffer info.
    if(info.elementSize == 0)
    {
        LogError() << "Invalid argument - \"elementSize\" is 0!";
        return false;
    }

    if(info.elementCount == 0)
    {
        LogError() << "Invalid argument - \"elementCount\" is 0!";
        return false;
    }

    // Setup a cleanup guard variable.
    bool initialized = false;

    // Create a buffer handle.
    glGenBuffers(1, &m_handle);

    if(m_handle == InvalidHandle)
    {
        LogError() << "Could not create a buffer handle!";
        return false;
    }

    SCOPE_GUARD_IF(!initialized, this->DestroyHandle());

    // Allocate buffer memory.
    unsigned int bufferSize = info.elementSize * info.elementCount;

    glBindBuffer(m_type, m_handle);
    glBufferData(m_type, bufferSize, info.data, info.usage);
    glBindBuffer(m_type, 0);

    // Save buffer parameters.
    m_elementSize = info.elementSize;
    m_elementCount = info.elementCount;

    LogInfo() << "Buffer size is " << bufferSize << " bytes.";

    // Success!
    Log() << "Success!";

    return initialized = true;
}

void Buffer::Update(const void* data, int count)
{
    Verify(m_handle != InvalidHandle, "Buffer handle has not been created!");
    Verify(data != nullptr, "Invalid argument - \"data\" is null!");
    Verify(count != 0, "Invalid argument - \"count\" is invalid!");

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

GLenum Buffer::GetType() const
{
    Verify(m_handle != InvalidHandle, "Buffer handle has not been created!");

    return m_type;
}

GLuint Buffer::GetHandle() const
{
    Verify(m_handle != InvalidHandle, "Buffer handle has not been created!");

    return m_handle;
}

unsigned int Buffer::GetElementSize() const
{
    Verify(m_handle != InvalidHandle, "Buffer handle has not been created!");

    return m_elementSize;
}

unsigned int Buffer::GetElementCount() const
{
    Verify(m_handle != InvalidHandle, "Buffer handle has not been created!");

    return m_elementCount;
}

GLenum Buffer::GetElementType() const
{
    Verify(m_handle != InvalidHandle, "Buffer handle has not been created!");

    return GL_INVALID_ENUM;
}

bool Buffer::IsValid() const
{
    return m_handle != 0;
}

bool Buffer::IsInstanced() const
{
    return false;
}

/*
    Vertex Buffer
*/

VertexBuffer::VertexBuffer() :
    Buffer(GL_ARRAY_BUFFER)
{
}

const char* VertexBuffer::GetName() const
{
    return "vertex buffer";
}

/*
    Index Buffer
*/

IndexBuffer::IndexBuffer() :
    Buffer(GL_ELEMENT_ARRAY_BUFFER)
{
}

const char* IndexBuffer::GetName() const
{
    return "index buffer";
}

GLenum IndexBuffer::GetElementType() const
{
    Verify(m_handle != InvalidHandle, "Buffer handle has not been created!");

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

/*
    Instance Buffer
*/

InstanceBuffer::InstanceBuffer() :
    Buffer(GL_ARRAY_BUFFER)
{
}

const char* InstanceBuffer::GetName() const
{
    return "instance buffer";
}

bool InstanceBuffer::IsInstanced() const
{
    return true;
}
