#pragma once

#include "Precompiled.hpp"
#include "VertexInput.hpp"
#include "Buffer.hpp"
#include "Sampler.hpp"
#include "Shader.hpp"
#include "Sprite.hpp"

// Forward declarations.
namespace System
{
    class ResourceManager;
}

/*
    Graphics Basic Renderer

    Handles basic drawing routines such as clearing the screen or drawing sprites.

    void ExampleBasicRenderer()
    {
        // Setup renderer info.
        Graphics::BasicRendererInfo basicRendererInfo;
        basicRendererInfo.resourceManager = &resourceManager;
        basicRendererInfo.spriteBatchSize = 128;

        // Create a basic renderer instance.
        Graphics::BasicRenderer basicRenderer;
        basicRenderer.Initialize(basicRendererInfo);

        // Clear the backbuffer.
        Graphics::ClearValues clearValues;
        clearValues.color = glm::vec4(0.0f, 0.35f, 0.35f, 1.0f);
        clearValues.depth = 1.0f;

        basicRenderer.Clear(clearValues);
    }
*/

namespace Graphics
{
    // Forward declarations.
    class Texture;

    // Clear values structure.
    struct ClearValues
    {
        std::optional<glm::vec4> color;
        std::optional<float> depth;
        std::optional<int> stencil;
    };

    // Basic renderer info struct.
    struct BasicRendererInfo
    {
        BasicRendererInfo();

        System::ResourceManager* resourceManager;
        int spriteBatchSize;
    };

    // Basic renderer class.
    class BasicRenderer
    {
    public:
        // Type declarations.
        typedef std::shared_ptr<const Shader> ShaderPtr;
        typedef std::vector<Sprite::Info> SpriteInfoList;
        typedef std::vector<Sprite::Data> SpriteDataList;

    public:
        BasicRenderer();
        ~BasicRenderer();

        // Initializes the basic renderer.
        bool Initialize(const BasicRendererInfo& info);

        // Clears the frame buffer.
        void Clear(const ClearValues& values);

        // Draws a single sprite.
        void DrawSprite(const Sprite& sprite, const glm::mat4& transform);

        // Draws a batch of sprites.
        // Provide sprite lists that are already sorted for most efficient rendering.
        void DrawSprites(const SpriteInfoList& spriteInfo, const SpriteDataList& spriteData, const glm::mat4& transform);

    private:
        // Graphics objects.
        VertexBuffer m_vertexBuffer;
        InstanceBuffer m_instanceBuffer;
        VertexInput m_vertexInput;
        Sampler m_nearestSampler;
        Sampler m_linearSampler;
        ShaderPtr m_shader;

        // Sprite batch size.
        int m_spriteBatchSize;

        // Initialization state.
        bool m_initialized;
    };
}
