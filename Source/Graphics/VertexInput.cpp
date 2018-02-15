#include "Precompiled.hpp"
#include "VertexInput.hpp"
#include "Buffer.hpp"
using namespace Graphics;

namespace
{
    // Gets the row size of a vertex attribute type.
    int GetVertexAttributeTypeRowSize(VertexAttributeTypes type)
    {
        switch(type)
        {
        case VertexAttributeTypes::Float1:
            return 1;

        case VertexAttributeTypes::Float2:
            return 2;

        case VertexAttributeTypes::Float3:
            return 3;

        case VertexAttributeTypes::Float4:
            return 4;

        case VertexAttributeTypes::Float4x4:
            return 4;

        default:
            Verify(false, "Unknown attribute type!");
            return 0;
        }
    }

    // Gets the row count of a vertex attribute type.
    int GetVertexAttributeTypeRowCount(VertexAttributeTypes type)
    {
        switch(type)
        {
        case VertexAttributeTypes::Float1:
        case VertexAttributeTypes::Float2:
        case VertexAttributeTypes::Float3:
        case VertexAttributeTypes::Float4:
            return 1;

        case VertexAttributeTypes::Float4x4:
            return 4;

        default:
            Verify(false, "Unknown attribute type!");
            return 0;
        }
    }

    // Gets the row offset of a vertex attribute type.
    int GetVertexAttributeTypeRowOffset(VertexAttributeTypes type)
    {
        switch(type)
        {
        case VertexAttributeTypes::Float1:
            return sizeof(float) * 1;

        case VertexAttributeTypes::Float2:
            return sizeof(float) * 2;

        case VertexAttributeTypes::Float3:
            return sizeof(float) * 3;

        case VertexAttributeTypes::Float4:
            return sizeof(float) * 4;

        case VertexAttributeTypes::Float4x4:
            return sizeof(float) * 4;

        default:
            Verify(false, "Unknown attribute type!");
            return 0;
        }
    }

    // Gets the OpenGL enum of a vertex attribute type.
    GLenum GetVertexAttributeTypeEnum(VertexAttributeTypes type)
    {
        switch(type)
        {
        case VertexAttributeTypes::Float1:
        case VertexAttributeTypes::Float2:
        case VertexAttributeTypes::Float3:
        case VertexAttributeTypes::Float4:
        case VertexAttributeTypes::Float4x4:
            return GL_FLOAT;

        default:
            Verify(false, "Unknown attribute type!");
            return GL_INVALID_ENUM;
        }
    }

    // Constant definitions.
    const GLuint InvalidHandle = 0;
}

VertexAttribute::VertexAttribute() :
    buffer(nullptr), type(VertexAttributeTypes::Invalid)
{
}

VertexAttribute::VertexAttribute(const Buffer* buffer, VertexAttributeTypes type) :
    buffer(buffer), type(type)
{
}

VertexInputInfo::VertexInputInfo() :
    attributes(nullptr),
    attributeCount(0)
{
}

VertexInputInfo::VertexInputInfo(const VertexAttribute* attributes, int attributeCount) :
    attributes(attributes), attributeCount(attributeCount)
{
}

VertexInput::VertexInput() :
    m_handle(InvalidHandle)
{
}

VertexInput::~VertexInput()
{
    this->DestroyHandle();
}

void VertexInput::DestroyHandle()
{
    // Release the vertex array handle.
    if(m_handle != InvalidHandle)
    {
        glDeleteVertexArrays(1, &m_handle);
        m_handle = InvalidHandle;
    }
}

bool VertexInput::Create(const VertexInputInfo& info)
{
    Log() << "Creating vertex input..." << LogIndent();

    // Check if handle has been already created.
    Verify(m_handle == InvalidHandle, "Vertex input instance has been already initialized!");

    // Validate arguments.
    if(info.attributeCount <= 0)
    {
        LogError() << "Invalid argument - \"count\" is zero!";
        return false;
    }

    if(info.attributes == nullptr)
    {
        LogError() << "Invalid argument - \"attributes\" is null!";
        return false;
    }

    for(int i = 0; i < info.attributeCount; ++i)
    {
        const VertexAttribute& attribute = info.attributes[i];

        if(attribute.buffer == nullptr)
        {
            LogError() << "Invalid argument - \"attribute[" << i << "].buffer\" is null!";
            return false;
        }

        if(!attribute.buffer->IsValid())
        {
            LogError() << "Invalid argument - \"attribute[" << i << "].buffer\" is invalid!";
            return false;
        }

        if(attribute.buffer->GetType() != GL_ARRAY_BUFFER)
        {
            LogError() << "Invalid argument - \"attribute[" << i << "].buffer\" is not a vertex or an instance buffer!";
            return false;
        }

        if(attribute.type == VertexAttributeTypes::Invalid)
        {
            LogError() << "Invalid argument - \"attribute[" << i << "].type\" is invalid!";
            return false;
        }
    }

    // Setup a cleanup guard variable.
    bool initialized = false;

    // Create a vertex array object.
    SCOPE_GUARD_IF(!initialized, this->DestroyHandle());

    glGenVertexArrays(1, &m_handle);

    if(m_handle == 0)
    {
        LogError() << "Could not create a vertex array handle!";
        return false;
    }

    Assert(glGetError() == GL_NO_ERROR, "OpenGL error has been encountered!");

    // Prepare a cleanup guard.
    SCOPE_GUARD_BEGIN();
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    SCOPE_GUARD_END();

    // Bind the vertex array handle.
    glBindVertexArray(m_handle);

    Assert(glGetError() == GL_NO_ERROR, "OpenGL error has been encountered!");

    // Set the vertex array's state.
    const Buffer* currentBuffer = nullptr;

    int currentLocation = 0;
    int currentOffset = 0;

    for(int i = 0; i < info.attributeCount; ++i)
    {
        const VertexAttribute& attribute = info.attributes[i];

        // Bind the vertex buffer.
        if(currentBuffer != attribute.buffer)
        {
            glBindBuffer(GL_ARRAY_BUFFER, attribute.buffer->GetHandle());

            Assert(glGetError() == GL_NO_ERROR, "OpenGL error has been encountered!");

            currentBuffer = attribute.buffer;
            currentOffset = 0;
        }

        // Setup vertex attributes for each used vertex location.
        for(int l = 0; l < GetVertexAttributeTypeRowCount(attribute.type); ++l)
        {
            // Enable vertex attribute.
            glEnableVertexAttribArray(currentLocation);

            Assert(glGetError() == GL_NO_ERROR, "OpenGL error has been encountered!");

            // Set vertex attribute pointer.
            glVertexAttribPointer(
                currentLocation,
                GetVertexAttributeTypeRowSize(attribute.type),
                GetVertexAttributeTypeEnum(attribute.type),
                GL_FALSE,
                attribute.buffer->GetElementSize(),
                (void*)currentOffset
            );

            Assert(glGetError() == GL_NO_ERROR, "OpenGL error has been encountered!");

            // Make vertex location instanced.
            if(attribute.buffer->IsInstanced())
            {
                glVertexAttribDivisor(currentLocation, 1);

                Assert(glGetError() == GL_NO_ERROR, "OpenGL error has been encountered!");
            }

            // Increment current location.
            currentLocation += 1;

            // Increment current offset.
            currentOffset += GetVertexAttributeTypeRowOffset(attribute.type);
        }
    }

    // Success!
    LogInfo() << "Success!";

    return initialized = true;
}

GLuint VertexInput::GetHandle() const
{
    Verify(m_handle != InvalidHandle, "Vertex array handle has not been created!");

    return m_handle;
}

bool VertexInput::IsValid() const
{
    return m_handle != InvalidHandle;
}
