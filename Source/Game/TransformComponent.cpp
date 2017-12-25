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
