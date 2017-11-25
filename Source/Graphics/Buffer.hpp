#pragma once

#include "Precompiled.hpp"

/*
    Buffer
    
    Generic object that can handle different types of OpenGL buffers.
    
    Creating a vertex buffer:
        struct Vertex
        {
            glm::vec3 position;
            glm::vec4 color;
        };
        
        const Vertex vertices[] =
        {
            { glm::vec3( 0.0f,  0.433f,  0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f) }, // Top
            { glm::vec3( 0.5f, -0.433f,  0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f) }, // Right
            { glm::vec3(-0.5f, -0.433f,  0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f) }, // Left
        };

        Graphics::BufferInfo bufferInfo;
        bufferInfo.elementSize = sizeof(Vertex);
        bufferInfo.elementCount = Utility::ArraySize(vertices);
        bufferInfo.data = &vertices[0];
        
        Graphics::VertexBuffer vertexBuffer;
        vertexBuffer.Create(bufferInfo);
*/

namespace Graphics
{
    // Buffer info structure.
    struct BufferInfo
    {
        BufferInfo();

        GLenum usage;
        unsigned int elementSize;
        unsigned int elementCount;
        const void* data;
    };

    // Buffer class.
    class Buffer
    {
    protected:
        Buffer(GLenum type);
        virtual ~Buffer();

    public:
        // Restores instance to its original state.
        void Cleanup();

        // Creates the buffer instance.
        bool Create(const BufferInfo& info);

        // Updates the buffer's data.
        void Update(const void* data, int count = -1);

        // Checks if the buffer is valid.
        bool IsValid() const
        {
            return m_handle != 0;
        }

        // Gets the buffer's type.
        GLenum GetType() const
        {
            return m_type;
        }

        // Gets the buffer's handle.
        GLuint GetHandle() const
        {
            return m_handle;
        }

        // Gets the buffer's element size.
        unsigned int GetElementSize() const
        {
            return m_elementSize;
        }

        // Gets the buffer's element count.
        unsigned int GetElementCount() const
        {
            return m_elementCount;
        }

        // Gets the buffer's element type.
        // For use in index buffers where element size indicates a data type.
        virtual GLenum GetElementType() const
        {
            return GL_INVALID_ENUM;
        }

        // Checks if the buffer is instanced.
        virtual bool IsInstanced() const
        {
            return false;
        }

        // Gets the buffer's name.
        virtual const char* GetName() const = 0;

    private:
        // Destroys the internal handle.
        void DestroyHandle();

    protected:
        // Buffer handle.
        GLenum m_type;
        GLuint m_handle;

        // Buffer parameters.
        unsigned int m_elementSize;
        unsigned int m_elementCount;
    };
}

/*
    Vertex Buffer
*/

namespace Graphics
{
    class VertexBuffer : public Buffer
    {
    public:
        VertexBuffer() :
            Buffer(GL_ARRAY_BUFFER)
        {
        }

        const char* GetName() const override
        {
            return "a vertex buffer";
        }
    };
}

/*
    Index Buffer
*/

namespace Graphics
{
    class IndexBuffer : public Buffer
    {
    public:
        IndexBuffer() :
            Buffer(GL_ELEMENT_ARRAY_BUFFER)
        {
        }

        GLenum GetElementType() const override;

        const char* GetName() const override
        {
            return "an index buffer";
        }
    };
}

/*
    Instance Buffer
*/

namespace Graphics
{
    class InstanceBuffer : public Buffer
    {
    public:
        InstanceBuffer() :
            Buffer(GL_ARRAY_BUFFER)
        {
        }

        bool IsInstanced() const override
        {
            return true;
        }

        const char* GetName() const override
        {
            return "an instance buffer";
        }
    };
}
