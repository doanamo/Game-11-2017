#pragma once

#include "Precompiled.hpp"
#include "Component.hpp"

//
// Transform Component
//

namespace Game
{
    namespace Components
    {
        // Transform component class.
        class Transform : public Component
        {
        public:
            Transform();
            ~Transform();

            // Sets the position.
            void SetPosition(const glm::vec3& position)
            {
                m_position = position;
            }

            void SetPosition(float x, float y, float z = 0.0f)
            {
                m_position.x = x;
                m_position.y = y;
                m_position.z = z;
            }

            // Sets the rotation.
            void SetRotation(const glm::vec3& rotation)
            {
                m_rotation = rotation;
            }

            void SetRotation(float x, float y, float z)
            {
                m_rotation.x = x;
                m_rotation.y = y;
                m_rotation.z = z;
            }

            // Sets the scale.
            void SetScale(const glm::vec3& scale)
            {
                m_scale = scale;
            }

            void SetScale(float x, float y, float z = 1.0f)
            {
                m_scale.x = x;
                m_scale.y = y;
                m_scale.z = z;
            }

            // Gets the position.
            const glm::vec3& GetPosition() const
            {
                return m_position;
            }

            // Gets the rotation.
            const glm::vec3& GetRotation() const
            {
                return m_rotation;
            }

            // Gets the scale.
            const glm::vec3& GetScale() const
            {
                return m_scale;
            }

        private:
            // Transform data.
            glm::vec3 m_position;
            glm::vec3 m_rotation;
            glm::vec3 m_scale;
        };
    }
}
