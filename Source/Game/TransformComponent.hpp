#pragma once

#include "Precompiled.hpp"
#include "Component.hpp"

/*
    Transform Component
*/

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
            void SetPosition(const glm::vec3& position);
            void SetPosition(float x, float y, float z = 0.0f);

            // Sets the rotation.
            void SetRotation(const glm::vec3& rotation);
            void SetRotation(float x, float y, float z);

            // Sets the scale.
            void SetScale(const glm::vec3& scale);
            void SetScale(float x, float y, float z = 1.0f);

            // Gets the position.
            const glm::vec3& GetPosition() const;

            // Gets the rotation.
            const glm::vec3& GetRotation() const;

            // Gets the scale.
            const glm::vec3& GetScale() const;

        private:
            // Transform data.
            glm::vec3 m_position;
            glm::vec3 m_rotation;
            glm::vec3 m_scale;
        };
    }
}
