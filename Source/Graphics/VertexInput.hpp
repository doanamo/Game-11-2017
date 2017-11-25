#pragma once

#include "Precompiled.hpp"

/*
    Vertex Input

    Creates a vertex layout that binds vertex buffers to shader inputs on the pipeline.

    Example usage:
        const Graphics::VertexAttribute vertexAttributes[] =
        {
            { &vertexBuffer, Graphics::VertexAttributeTypes::Float2 }, // Position
            { &vertexBuffer, Graphics::VertexAttributeTypes::Float2 }, // Texture
            { &vertexBuffer, Graphics::VertexAttributeTypes::Float4 }, // Color
        };
    
        Graphics::VertexInput vertexInput;
        vertexInput.Initialize(Utility::ArraySize(vertexAttributes), &vertexAttributes[0]);
    
        glBindVertexArray(vertexInput.GetHandle());
*/

namespace Graphics
{
    // Forward declarations.
    class Buffer;

    // Vertex attribute types.
    enum class VertexAttributeTypes
    {
        Invalid,

        Float1,
        Float2,
        Float3,
        Float4,

        Float4x4,

        Count,
    };

    // Vertex attribute structure.
    struct VertexAttribute
    {
        VertexAttribute();
        VertexAttribute(const Buffer* buffer, VertexAttributeTypes type);

        const Buffer* buffer;
        VertexAttributeTypes type;
    };

    // Vertex input info structure.
    struct VertexInputInfo
    {
        VertexInputInfo();
        VertexInputInfo(const VertexAttribute* attributes, int attributeCount);

        const VertexAttribute* attributes;
        int attributeCount;
    };

    // Vertex input class.
    class VertexInput
    {
    public:
        VertexInput();
        ~VertexInput();

        // Restores instance to it's original state.
        void Cleanup();

        // Initializes the vertex input instance.
        bool Create(const VertexInputInfo& info);

        // Gets the vertex array object handle.
        GLuint GetHandle() const
        {
            return m_handle;
        }

        // Checks if instance is valid.
        bool IsValid() const;

    private:
        // Destroys the internal handle.
        void DestroyHandle();

    private:
        // Vertex array handle.
        GLuint m_handle;
    };
}