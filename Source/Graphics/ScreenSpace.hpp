#pragma once

#include "Precompiled.hpp"

/*
    Screen Space
    
    Creates a screen space transform with an orthogonal projection.
    Can be used for interface drawing or orthogonal 2D view.
    
    Requires targets size (e.g. viewport or window) and source size to be
    specified (i.e. screen coordinates). The source will be enclosed inside the target
    and it will also be scaled if the target size becomes smaller. The important
    thing to point out is that the specified source, even when scaled to fit
    the target, will always preserves its aspect ratio and coordinate space.
    
    Defining screen space coordinates:
        Graphics::ScreenSpace screenSpace;
        screenSpace.SetTargetSize(windowWidth, windowHeight);
        screenSpace.SetSourceSize(1024.0f, 576.0f);
        
        glViewport(0, 0, windowWidth, windowHeight);
        glm::mat4 transform = screenSpace.GetTransform();
*/

namespace Graphics
{
    // Screen space class.
    class ScreenSpace
    {
    public:
        ScreenSpace();
        ~ScreenSpace();

        // Sets the target size.
        // Needs to be updated evey time the target resizes.
        void SetTargetSize(int width, int height);

        // Sets the source size.
        void SetSourceSize(float width, float height);

        // Sets the source size aspect ratio that will be preserved when scaled.
        // Aspect is equal to horizontal width divided by vertical height.
        void SetSourceAspectRatio(float aspectRatio);

        // Gets the target size.
        const glm::vec2& GetTargetSize() const;

        // Gets the source size.
        const glm::vec2& GetSourceSize() const;

        // Gets the target rectangle.
        // Returns a [left, right, bottom, top] vector.
        const glm::vec4& GetRectangle() const;

        // Gets the offset from the center to bottom left corner of the source.
        // Can be used to move the origin to the center.
        const glm::vec2& GetOffset() const;

        // Gets the projection matrix.
        const glm::mat4& GetProjection() const;

        // Gets the view matrix.
        const glm::mat4& GetView() const;

        // Gets the combined projection and view matrices.
        const glm::mat4& GetTransform() const;

    private:
        glm::vec2 m_targetSize;

        mutable glm::vec2 m_sourceSize;
        float m_sourceAspectRatio;

        mutable glm::vec4 m_rectangle;
        mutable glm::vec2 m_offset;

        mutable glm::mat4 m_projection;
        mutable glm::mat4 m_view;
        mutable glm::mat4 m_transform;

    private:
        void Rebuild() const;

        mutable bool m_rebuild;
        mutable bool m_rebuildSourceSize;
    };
}
