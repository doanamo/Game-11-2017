#include "Precompiled.hpp"
#include "BasicRenderer.hpp"
#include "System/ResourceManager.hpp"
#include "Graphics/Texture.hpp"
using namespace Graphics;

namespace
{
    // Vertex structure.
    struct Vertex
    {
        glm::vec2 position;
        glm::vec2 texture;
    };
}

BasicRendererInfo::BasicRendererInfo() :
    resourceManager(nullptr),
    spriteBatchSize(128)
{
}

BasicRenderer::BasicRenderer() :
    m_spriteBatchSize(0),
    m_initialized(false)
{
}

BasicRenderer::~BasicRenderer()
{
}

bool BasicRenderer::Initialize(const BasicRendererInfo& info)
{
    Log() << "Initializing basic renderer..." << LogIndent();

    // Check if instance has been already initialized.
    Verify(!m_initialized, "Basic renderer instance has been already initialized!");

    // Validate arguments.
    if(info.resourceManager == nullptr)
    {
        LogError() << "Invalid argument - \"resourceManager\" is null!";
        return false;
    }

    if(info.spriteBatchSize <= 0)
    {
        LogError() << "Invalid argument - \"spriteBatchSize\" is invalid!";
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
        LogError() << "Could not create a vertex buffer!";
        return false;
    }

    SCOPE_GUARD_IF(!m_initialized, m_vertexBuffer = VertexBuffer());

    // Create an instance buffer.
    BufferInfo instanceBufferInfo;
    instanceBufferInfo.usage = GL_DYNAMIC_DRAW;
    instanceBufferInfo.elementSize = sizeof(Sprite::Data);
    instanceBufferInfo.elementCount = info.spriteBatchSize;
    instanceBufferInfo.data = nullptr;

    if(!m_instanceBuffer.Create(instanceBufferInfo))
    {
        LogError() << "Could not create an instance buffer!";
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
        LogError() << "Could not create a vertex input!";
        return false;
    }

    SCOPE_GUARD_IF(!m_initialized, m_vertexInput = VertexInput());

    // Create a nearest filtering sampler.
    SamplerInfo nearestSamplerInfo;
    nearestSamplerInfo.textureMinFilter = GL_NEAREST;
    nearestSamplerInfo.textureMagFilter = GL_NEAREST;

    if(!m_nearestSampler.Create(nearestSamplerInfo))
    {
        LogError() << "Could not create a nearest filtering sampler!";
        return false;
    }

    SCOPE_GUARD_IF(!m_initialized, m_nearestSampler = Sampler());

    // Create a linear filtering sampler.
    SamplerInfo linearSamplerInfo;
    linearSamplerInfo.textureMinFilter = GL_LINEAR;
    linearSamplerInfo.textureMagFilter = GL_LINEAR;

    if(!m_linearSampler.Create(linearSamplerInfo))
    {
        LogError() << "Could not create a linear filtering sampler!";
        return false;
    }

    SCOPE_GUARD_IF(!m_initialized, m_linearSampler = Sampler());

    // Load the sprite shader.
    m_shader = info.resourceManager->Load<Shader>("Data/Shaders/Sprite.glsl");

    if(!m_shader->IsValid())
    {
        LogError() << "Could not load the sprite shader!";
        return false;
    }

    SCOPE_GUARD_IF(!m_initialized, m_shader = nullptr);

    // Remember the sprite batch size.
    m_spriteBatchSize = info.spriteBatchSize;

    // Success!
    LogInfo() << "Success!";

    return m_initialized = true;
}

void BasicRenderer::Clear(const ClearValues& values)
{
    Verify(m_initialized, "Instance has not been initialized!");

    // Set clearing masks.
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

    // Clear the frame buffer.
    glClear(mask);
}

void BasicRenderer::DrawSprite(const Sprite& sprite, const::glm::mat4& transform)
{
    Verify(m_initialized, "Instance has not been initialized!");

    // Create sprite lists.
    SpriteInfoList spriteInfo({ sprite.info });
    SpriteDataList spriteData({ sprite.data });

    // Call the sprite batching method with only one sprite.
    this->DrawSprites(spriteInfo, spriteData, transform);
}

void BasicRenderer::DrawSprites(const SpriteInfoList& spriteInfo, const SpriteDataList& spriteData, const glm::mat4& transform)
{
    Verify(m_initialized, "Instance has not been initialized!");

    // Make sure both lists have the same size.
    Verify(spriteInfo.size() == spriteData.size(), "Sprite data and info lists have different sizes!");

    // Bind the vertex input.
    glBindVertexArray(m_vertexInput.GetHandle());

    SCOPE_GUARD(glBindVertexArray(0));

    // Bind shader program.
    glUseProgram(m_shader->GetHandle());

    SCOPE_GUARD(glUseProgram(0));

    // Set the view transform.
    glUniformMatrix4fv(m_shader->GetUniform("viewTransform"), 1, GL_FALSE, glm::value_ptr(transform));

    // Current transparency state.
    bool currentTransparent = false;

    SCOPE_GUARD_BEGIN();
    {
        if(currentTransparent)
        {
            glDisable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ZERO);

            glDepthMask(GL_TRUE);
        }
    }
    SCOPE_GUARD_END();

    // Current texture state.
    const Texture* currentTexture = nullptr;

    SCOPE_GUARD_BEGIN();
    {
        if(currentTexture != nullptr)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);

            glBindSampler(0, 0);
        }
    }
    SCOPE_GUARD_END();

    // Set used texture slot.
    glUniform1i(m_shader->GetUniform("textureDiffuse"), 0);

    // Render sprite batches.
    const int spriteCount = spriteInfo.size();
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
            if(spritesBatched == m_spriteBatchSize)
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
                // Set default blend state.
                glDisable(GL_BLEND);

                // Set default blend mask.
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
