#include "Precompiled.hpp"
#include "VertexInput.hpp"
#include "Buffer.hpp"
using namespace Graphics;

namespace
{
    // Log error messages.
    #define LogCreateError() "Failed to create a vertex input! "

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
            Assert(false, "Unknown attribute type.");
        }

        return 0;
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
            Assert(false, "Unknown attribute type.");
        }

        return 0;
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
            Assert(false, "Unknown attribute type.");
        }

        return 0;
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
            Assert(false, "Unknown attribute type.");
        }

        return GL_INVALID_ENUM;
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
    // Check if handle has been already created.
    if(m_handle != InvalidHandle)
    {
        Log() << LogCreateError() << "Vertex input instance has been already initialized.";
        return false;
    }

    // Validate arguments.
    if(info.attributeCount <= 0)
    {
        Log() << LogCreateError() << "Invalid argument - \"count\" is 0.";
        return false;
    }

    if(info.attributes == nullptr)
    {
        Log() << LogCreateError() << "Invalid argument - \"attributes\" is null.";
        return false;
    }

    for(int i = 0; i < info.attributeCount; ++i)
    {
        const VertexAttribute& attribute = info.attributes[i];

        if(attribute.buffer == nullptr)
        {
            Log() << LogCreateError() << "Invalid argument - \"attribute[" << i << "].buffer\" is null.";
            return false;
        }

        if(!attribute.buffer->IsValid())
        {
            Log() << LogCreateError() << "Invalid argument - \"attribute[" << i << "].buffer\" is invalid.";
            return false;
        }

        if(attribute.buffer->GetType() != GL_ARRAY_BUFFER)
        {
            Log() << LogCreateError() << "Invalid argument - \"attribute[" << i << "].buffer\" is not a vertex or an instance buffer.";
            return false;
        }

        if(attribute.type == VertexAttributeTypes::Invalid)
        {
            Log() << LogCreateError() << "Invalid argument - \"attribute[" << i << "].type\" is invalid.";
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
        Log() << LogCreateError() << "Could not create a vertex array object.";
        return false;
    }

    // Bind the vertex array handle.
    glBindVertexArray(m_handle);

    SCOPE_GUARD
    (
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    );

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

            currentBuffer = attribute.buffer;
            currentOffset = 0;
        }

        // Setup vertex attributes for each used vertex location.
        for(int l = 0; l < GetVertexAttributeTypeRowCount(attribute.type); ++l)
        {
            // Enable vertex attribute.
            glEnableVertexAttribArray(currentLocation);

            // Set vertex attribute pointer.
            glVertexAttribPointer(
                currentLocation,
                GetVertexAttributeTypeRowSize(attribute.type),
                GetVertexAttributeTypeEnum(attribute.type),
                GL_FALSE,
                attribute.buffer->GetElementSize(),
                (void*)currentOffset
            );

            // Make vertex location instanced.
            if(attribute.buffer->IsInstanced())
            {
                glVertexAttribDivisor(currentLocation, 1);
            }

            // Increment current location.
            currentLocation += 1;

            // Increment current offset.
            currentOffset += GetVertexAttributeTypeRowOffset(attribute.type);
        }
    }

    // Success!
    return initialized = true;
}

bool VertexInput::IsValid() const
{
    return m_handle != InvalidHandle;
}
