#include "Precompiled.hpp"
#include "RenderComponent.hpp"
#include "TransformComponent.hpp"
#include "Graphics/Texture.hpp"
using namespace Game::Components;

Render::Render() :
    m_rectangle(0.0f, 0.0f, 1.0f, 1.0f),
    m_diffuseColor(1.0f, 1.0f, 1.0f, 1.0f),
    m_emissiveColor(1.0f, 1.0f, 1.0f, 1.0f),
    m_emissivePower(0.0f),
    m_transparent(true),
    m_transform(nullptr)
{
}

Render::~Render()
{
}

glm::vec4 Render::CalculateColor() const
{
    return glm::mix(m_diffuseColor, m_emissiveColor, m_emissivePower);
}

void Render::SetTexture(TexturePtr texture)
{
    if(texture == nullptr)
        return;

    m_texture = texture;
    m_rectangle = glm::vec4(0.0f, 0.0f, texture->GetWidth(), texture->GetHeight());
}

void Render::SetRectangle(const glm::vec4& rectangle)
{
    m_rectangle = rectangle;
}

void Render::SetRectangle(float x, float y, float width, float height)
{
    m_rectangle.x = x;
    m_rectangle.y = y;
    m_rectangle.z = width;
    m_rectangle.w = height;
}

void Render::SetDiffuseColor(const glm::vec3& color)
{
    m_diffuseColor = glm::vec4(color, 1.0f);
}

void Render::SetDiffuseColor(const glm::vec4& color)
{
    m_diffuseColor = color;
}

void Render::SetDiffuseColor(float r, float g, float b, float a)
{
    m_diffuseColor.r = r;
    m_diffuseColor.g = g;
    m_diffuseColor.b = b;
    m_diffuseColor.a = a;
}

void Render::SetEmissiveColor(const glm::vec3& color)
{
    m_emissiveColor = glm::vec4(color, 1.0f);
}

void Render::SetEmissiveColor(const glm::vec4& color)
{
    m_emissiveColor = color;
}

void Render::SetEmissiveColor(float r, float g, float b, float a)
{
    m_emissiveColor.r = r;
    m_emissiveColor.g = g;
    m_emissiveColor.b = b;
    m_emissiveColor.a = a;
}

void Render::SetEmissivePower(float power)
{
    m_emissivePower = power;
}

void Render::SetTransparent(bool transparent)
{
    m_transparent = transparent;
}

const Render::TexturePtr& Render::GetTexture() const
{
    return m_texture;
}

const glm::vec4& Render::GetRectangle() const
{
    return m_rectangle;
}

const glm::vec4& Render::GetDiffuseColor() const
{
    return m_diffuseColor;
}

const glm::vec4& Render::GetEmissiveColor() const
{
    return m_emissiveColor;
}

float Render::GetEmissivePower() const
{
    return m_emissivePower;
}

bool Render::IsTransparent() const
{
    return m_transparent;
}

Transform* Render::GetTransform()
{
    return m_transform;
}
