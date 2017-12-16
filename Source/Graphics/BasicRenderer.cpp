#include "Precompiled.hpp"
#include "BasicRenderer.hpp"
#include "System/ResourceManager.hpp"
#include "Graphics/Texture.hpp"
using namespace Graphics;

namespace
{
    // Log error messages.
    #define LogInitializeError() "Failed to initialize the basic renderer! "

    // Vertex structure.
    struct Vertex
    {
        glm::vec2 position;
        glm::vec2 texture;
    };
}

BasicRenderer::BasicRenderer() :
    m_initialized(false)
{
    // Make sure we have a valid sprite batch size.
    static_assert(SpriteBatchSize >= 1, "Invalid sprite batch size.");
}

BasicRenderer::~BasicRenderer()
{
}

bool BasicRenderer::Initialize(System::ResourceManager& resourceManager)
{
    // Check if handle has been already created.
    if(m_initialized)
    {
        Log() << LogInitializeError() << "Instance has been already initialized.";
        return false;
    }

    // Create a vertex buffer.
    const Vertex vertices[4] =
    {
        { glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f) }, // Bottom-Left
        { glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 1.0f) }, // Bottom-Right
        { glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 0.0f) }, // Top-Left
        { glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f) }, // Top-Right
    };

    BufferInfo vertexBufferInfo;
    vertexBufferInfo.usage = GL_STATIC_DRAW;
    vertexBufferInfo.elementSize = sizeof(Vertex);
    vertexBufferInfo.elementCount = Utility::ArraySize(vertices);
    vertexBufferInfo.data = &vertices[0];

    if(!m_vertexBuffer.Create(vertexBufferInfo))
    {
        Log() << LogInitializeError() << "Could not create a vertex buffer.";
        return false;
    }

    SCOPE_GUARD_IF(!m_initialized, m_vertexBuffer = VertexBuffer());

    // Create an instance buffer.
    BufferInfo instanceBufferInfo;
    instanceBufferInfo.usage = GL_DYNAMIC_DRAW;
    instanceBufferInfo.elementSize = sizeof(Sprite::Data);
    instanceBufferInfo.elementCount = SpriteBatchSize;
    instanceBufferInfo.data = nullptr;

    if(!m_instanceBuffer.Create(instanceBufferInfo))
    {
        Log() << LogInitializeError() << "Could not create an instance buffer.";
        return false;
    }

    SCOPE_GUARD_IF(!m_initialized, m_instanceBuffer = InstanceBuffer());

    // Create a vertex input.
    const VertexAttribute vertexAttributes[] =
    {
        { &m_vertexBuffer,   VertexAttributeTypes::Float2   }, // Position
        { &m_vertexBuffer,   VertexAttributeTypes::Float2   }, // Texture
        { &m_instanceBuffer, VertexAttributeTypes::Float4x4 }, // Transform
        { &m_instanceBuffer, VertexAttributeTypes::Float4   }, // Rectangle
        { &m_instanceBuffer, VertexAttributeTypes::Float4   }, // Color
    };

    VertexInputInfo vertexInputInfo;
    vertexInputInfo.attributeCount = Utility::ArraySize(vertexAttributes);
    vertexInputInfo.attributes = &vertexAttributes[0];

    if(!m_vertexInput.Create(vertexInputInfo))
    {
        Log() << LogInitializeError() << "Could not create a vertex input.";
        return false;
    }

    SCOPE_GUARD_IF(!m_initialized, m_vertexInput = VertexInput());

    // Create a nearest filtering sampler.
    SamplerInfo nearestSamplerInfo;
    nearestSamplerInfo.textureMinFilter = GL_NEAREST;
    nearestSamplerInfo.textureMagFilter = GL_NEAREST;

    if(!m_nearestSampler.Create(nearestSamplerInfo))
    {
        Log() << LogInitializeError() << "Could not create a nearest filtering sampler.";
        return false;
    }

    SCOPE_GUARD_IF(!m_initialized, m_nearestSampler = Sampler());

    // Create a linear filtering sampler.
    SamplerInfo linearSamplerInfo;
    linearSamplerInfo.textureMinFilter = GL_LINEAR;
    linearSamplerInfo.textureMagFilter = GL_LINEAR;

    if(!m_linearSampler.Create(linearSamplerInfo))
    {
        Log() << LogInitializeError() << "Could not create a linear filtering sampler.";
        return false;
    }

    SCOPE_GUARD_IF(!m_initialized, m_linearSampler = Sampler());

    // Load the sprite shader.
    m_shader = resourceManager.Load<Shader>("Data/Shaders/Sprite.glsl");

    if(!m_shader->IsValid())
    {
        Log() << LogInitializeError() << "Could not load the sprite shader.";
        return false;
    }

    SCOPE_GUARD_IF(!m_initialized, m_shader = nullptr);

    // Success!
    return m_initialized = true;
}

void BasicRenderer::Clear(const ClearValues& values)
{
    if(!m_initialized)
        return;

    // Clear the frame buffer.
    GLbitfield mask = GL_NONE;

    if(values.color.has_value())
    {
        const glm::vec4& color = values.color.value();
        glClearColor(color.r, color.g, color.b, color.a);
        mask |= GL_COLOR_BUFFER_BIT;
    }

    if(values.depth.has_value())
    {
        glClearDepth(values.depth.value());
        mask |= GL_DEPTH_BUFFER_BIT;
    }

    if(values.stencil.has_value())
    {
        glClearStencil(values.stencil.value());
        mask |= GL_STENCIL_BUFFER_BIT;
    }

    glClear(mask);
}

void BasicRenderer::DrawSprite(const Sprite& sprite, const::glm::mat4& transform)
{
    if(!m_initialized)
        return;

    // Bind the vertex input.
    glBindVertexArray(m_vertexInput.GetHandle());

    SCOPE_GUARD
    (
        glBindVertexArray(0);
    );

    // Bind shader program.
    glUseProgram(m_shader->GetHandle());

    SCOPE_GUARD
    (
        glUseProgram(0);
    );

    glUniformMatrix4fv(m_shader->GetUniform("viewTransform"), 1, GL_FALSE, glm::value_ptr(transform));

    // Setup transparency.
    if(sprite.info.transparent)
    {
        // Enable alpha blending.
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Disable depth writing.
        glDepthMask(GL_FALSE);
    }

    SCOPE_GUARD
    (
        if(sprite.info.transparent)
        {
            glDisable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ZERO);

            glDepthMask(GL_TRUE);
        }
    );

    // Setup texture.
    if(sprite.info.texture != nullptr)
    {
        // Calculate inversed texture size.
        glm::vec2 textureInvSize;
        textureInvSize.x = 1.0f / sprite.info.texture->GetWidth();
        textureInvSize.y = 1.0f / sprite.info.texture->GetHeight();

        glUniform2fv(m_shader->GetUniform("textureSizeInv"), 1, glm::value_ptr(textureInvSize));

        // Bind texture unit.
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sprite.info.texture->GetHandle());

        // Bind texture sampler.
        if(sprite.info.filter)
        {
            glBindSampler(0, m_linearSampler.GetHandle());
        }
        else
        {
            glBindSampler(0, m_nearestSampler.GetHandle());
        }
    }

    SCOPE_GUARD
    (
        if(sprite.info.texture != nullptr)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);

            glBindSampler(0, 0);
        }
    );

    glUniform1i(m_shader->GetUniform("textureDiffuse"), 0);

    // Update the instance buffer with sprite data.
    m_instanceBuffer.Update(&sprite.data, 1);

    // Draw instanced sprite.
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 1);
}

void BasicRenderer::DrawSprites(const SpriteInfoList& spriteInfo, const SpriteDataList& spriteData, const glm::mat4& transform)
{
    if(!m_initialized)
        return;

    // Make sure both lists have the same size.
    if(spriteInfo.size() != spriteData.size())
    {
        Log() << "Invalid arguments passed - sprite structures have different sizes!";
        return;
    }

    const int spriteCount = spriteInfo.size();

    // Bind the vertex input.
    glBindVertexArray(m_vertexInput.GetHandle());

    SCOPE_GUARD
    (
        glBindVertexArray(0);
    );

    // Bind shader program.
    glUseProgram(m_shader->GetHandle());

    SCOPE_GUARD
    (
        glUseProgram(0);
    );

    glUniformMatrix4fv(m_shader->GetUniform("viewTransform"), 1, GL_FALSE, glm::value_ptr(transform));

    // Current transparency state.
    bool currentTransparent = false;

    SCOPE_GUARD
    (
        if(currentTransparent)
        {
            glDisable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ZERO);

            glDepthMask(GL_TRUE);
        }
    );

    // Current texture state.
    const Texture* currentTexture = nullptr;

    SCOPE_GUARD
    (
        if(currentTexture != nullptr)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);

            glBindSampler(0, 0);
        }
    );

    glUniform1i(m_shader->GetUniform("textureDiffuse"), 0);

    // Render sprite batches.
    int spritesDrawn = 0;

    while(spritesDrawn != spriteCount)
    {
        // Get the next sprite info that will represent current batch.
        const Sprite::Info& info = spriteInfo[spritesDrawn];

        // Add similar sprites to the current batch, so we can draw them in a single call.
        int spritesBatched = 1;

        while(true)
        {
            // Check if we have reached the maximum batch size.
            if(spritesBatched == SpriteBatchSize)
                break;

            // Get the index of the next sprite.
            int spriteNext = spritesDrawn + spritesBatched;

            // Check if we have reached the end of the sprite list.
            if(spriteNext >= spriteCount)
                break;

            // Check if the next sprite can be batched.
            if(info != spriteInfo[spriteNext])
                break;

            // Add sprite to the batch.
            ++spritesBatched;
        }

        // Update the instance buffer with sprite data.
        m_instanceBuffer.Update(&spriteData[spritesDrawn], spritesBatched);

        // Set transparency state.
        if(currentTransparent != info.transparent)
        {
            if(info.transparent)
            {
                // Enable alpha blending.
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                // Disable depth writing.
                glDepthMask(GL_FALSE);
            }
            else
            {
                // Disable alpha blending.
                glDisable(GL_BLEND);

                // Enable depth writing.
                glDepthMask(GL_TRUE);
            }

            currentTransparent = info.transparent;
        }

        // Set texture state.
        if(currentTexture != info.texture)
        {
            // Set texture shader uniform.
            if(info.texture != nullptr)
            {
                // Calculate inversed texture size.
                glm::vec2 textureInvSize;
                textureInvSize.x = 1.0f / info.texture->GetWidth();
                textureInvSize.y = 1.0f / info.texture->GetHeight();

                glUniform2fv(m_shader->GetUniform("textureSizeInv"), 1, glm::value_ptr(textureInvSize));

                // Bind texture unit.
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, info.texture->GetHandle());

                // Bind texture sampler.
                if(info.filter)
                {
                    glBindSampler(0, m_linearSampler.GetHandle());
                }
                else
                {
                    glBindSampler(0, m_nearestSampler.GetHandle());
                }
            }
            else
            {
                // Disable texture unit.
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, 0);
            }

            currentTexture = info.texture;
        }

        // Draw instanced sprite batch.
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, spritesBatched);

        // Update the counter of drawn sprites.
        spritesDrawn += spritesBatched;
    }
}
