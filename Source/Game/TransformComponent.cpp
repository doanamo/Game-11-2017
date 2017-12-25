#include "Precompiled.hpp"
#include "TransformComponent.hpp"
using namespace Game::Components;

Transform::Transform() :
    m_position(0.0f, 0.0f, 0.0f),
    m_rotation(0.0f, 0.0f, 0.0f),
    m_scale(1.0f, 1.0f, 1.0f)
{
}

Transform::~Transform()
{
}

void Transform::SetPosition(const glm::vec3& position)
{
    m_position = position;
}

void Transform::SetPosition(float x, float y, float z)
{
    m_position.x = x;
    m_position.y = y;
    m_position.z = z;
}

void Transform::SetRotation(const glm::vec3& rotation)
{
    m_rotation = rotation;
}

void Transform::SetRotation(float x, float y, float z)
{
    m_rotation.x = x;
    m_rotation.y = y;
    m_rotation.z = z;
}

void Transform::SetScale(const glm::vec3& scale)
{
    m_scale = scale;
}

void Transform::SetScale(float x, float y, float z)
{
    m_scale.x = x;
    m_scale.y = y;
    m_scale.z = z;
}

const glm::vec3& Transform::GetPosition() const
{
    return m_position;
}

const glm::vec3& Transform::GetRotation() const
{
    return m_rotation;
}

const glm::vec3& Transform::GetScale() const
{
    return m_scale;
}
