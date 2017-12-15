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
    Basic Renderer

    Handles basic drawing routines.
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

    // Basic renderer class.
    class BasicRenderer
    {
    public:
        // Type declarations.
        typedef std::shared_ptr<const Shader> ShaderPtr;
        typedef std::vector<Sprite::Info> SpriteInfoList;
        typedef std::vector<Sprite::Data> SpriteDataList;

        // Constant variables.
        static const int SpriteBatchSize = 128;

    public:
        BasicRenderer();
        ~BasicRenderer();

        // Initializes the basic renderer.
        bool Initialize(System::ResourceManager& resourceManager);

        // Clears the frame buffer.
        void Clear(const ClearValues& values);

        // Draws a single sprite.
        void DrawSprite(const Sprite& sprite, const glm::mat4& transform);

        // Draws a batch of sprites.
        // Provide sprite lists that are already sorted for most efficient rendering.
        void DrawSprites(const SpriteInfoList& spriteInfo, const SpriteDataList& spriteData, const glm::mat4& transform);

    private:
        // Graphics objects.
        VertexBuffer   m_vertexBuffer;
        InstanceBuffer m_instanceBuffer;
        VertexInput    m_vertexInput;
        Sampler        m_nearestSampler;
        Sampler        m_linearSampler;
        ShaderPtr      m_shader;

        // Initialization state.
        bool m_initialized;
    };
}
