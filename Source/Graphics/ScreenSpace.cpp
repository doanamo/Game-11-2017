#include "Precompiled.hpp"
#include "ScreenSpace.hpp"
using namespace Graphics;

namespace
{
    // Floors a value and makes it a multiple of 2.
    template<typename Type>
    float FloorMultipleTwo(Type value)
    {
        int floor = (int)value;
        if(floor % 2) floor += 1;
        return (float)floor;
    }
}

ScreenSpace::ScreenSpace() :
    m_targetSize(4.0f, 4.0f),
    m_sourceSize(4.0f, 4.0f),
    m_sourceAspectRatio(1.0f),
    m_rectangle(0.0f, 0.0f, 0.0f, 0.0f),
    m_offset(0.0f, 0.0f),
    m_projection(1.0f),
    m_view(1.0f),
    m_transform(1.0f),
    m_rebuild(true),
    m_rebuildSourceSize(false)
{
}

ScreenSpace::~ScreenSpace()
{
}

void ScreenSpace::SetTargetSize(int width, int height)
{
    m_targetSize.x = (float)width;
    m_targetSize.y = (float)height;

    m_rebuild = true;
}

void ScreenSpace::SetSourceSize(float width, float height)
{
    m_sourceSize.x = width;
    m_sourceSize.y = height;
    m_sourceAspectRatio = width / height;

    m_rebuildSourceSize = false;
    m_rebuild = true;
}

void ScreenSpace::SetSourceAspectRatio(float aspectRatio)
{
    m_sourceSize.x = 0.0f;
    m_sourceSize.y = 0.0f;
    m_sourceAspectRatio = aspectRatio;

    m_rebuildSourceSize = true;
    m_rebuild = true;
}

void ScreenSpace::Rebuild() const
{
    if(m_rebuild)
    {
        // Floor the target size and make a multiple of 2.
        // This ensures that target and source sizes are uniform after possible integer to float conversions.
        m_targetSize.x = FloorMultipleTwo(m_targetSize.x);
        m_targetSize.y = FloorMultipleTwo(m_targetSize.y);

        // Build source size if needed.
        if(m_rebuildSourceSize)
        {
            // Set source size equal to target size.
            m_sourceSize = m_targetSize;

            // Adjust source size aspect ratio.
            float targetAspectRatio = m_targetSize.x / m_targetSize.y;

            if(targetAspectRatio > m_sourceAspectRatio)
            {
                m_sourceSize.x = m_targetSize.x / (targetAspectRatio / m_sourceAspectRatio);
            }
            else
            {
                m_sourceSize.y = m_targetSize.y * (targetAspectRatio / m_sourceAspectRatio);
            }   
        }

        // Floor the source size and make a multiple of 2.
        // This ensures that target and source sizes are uniform after possible integer to float conversions.
        m_sourceSize.x = FloorMultipleTwo(m_sourceSize.x);
        m_sourceSize.y = FloorMultipleTwo(m_sourceSize.y);

        // Calculate aspect ratios.
        float targetAspectRatio = m_targetSize.x / m_targetSize.y;
        float sourceAspectRatio = m_sourceSize.x / m_sourceSize.y;

        float aspectRatio = targetAspectRatio / sourceAspectRatio;

        // Calculate screen space coordinates.
        m_rectangle.x = -m_sourceSize.x * 0.5f; // Left
        m_rectangle.y =  m_sourceSize.x * 0.5f; // Right
        m_rectangle.z = -m_sourceSize.y * 0.5f; // Bottom
        m_rectangle.w =  m_sourceSize.y * 0.5f; // Top

        // Scale screen space coordinates.
        if(targetAspectRatio > sourceAspectRatio)
        {
            m_rectangle.x *= aspectRatio;
            m_rectangle.y *= aspectRatio;
        }
        else
        {
            m_rectangle.z /= aspectRatio;
            m_rectangle.w /= aspectRatio;
        }

        // Calculate screen space offset.
        m_offset.x = -m_sourceSize.x * 0.5f;
        m_offset.y = -m_sourceSize.y * 0.5f;

        // Calculate screen space matrices.
        m_projection = glm::ortho(m_rectangle.x, m_rectangle.y, m_rectangle.z, m_rectangle.w);
        m_view = glm::translate(glm::mat4(1.0f), glm::vec3(m_offset, 0.0f));
        m_transform = m_projection * m_view;

        m_rebuild = false;
    }
}

const glm::vec2& ScreenSpace::GetTargetSize() const
{
    return m_targetSize;
}

const glm::vec2& ScreenSpace::GetSourceSize() const
{
    return m_sourceSize;
}

const glm::vec4& ScreenSpace::GetRectangle() const
{
    this->Rebuild();
    return m_rectangle;
}

const glm::vec2& ScreenSpace::GetOffset() const
{
    this->Rebuild();
    return m_offset;
}

const glm::mat4& ScreenSpace::GetProjection() const
{
    this->Rebuild();
    return m_projection;
}

const glm::mat4& ScreenSpace::GetView() const
{
    this->Rebuild();
    return m_view;
}

const glm::mat4& ScreenSpace::GetTransform() const
{
    this->Rebuild();
    return m_transform;
}
