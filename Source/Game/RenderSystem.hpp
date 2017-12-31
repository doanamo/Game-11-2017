#pragma once

#include "Precompiled.hpp"
#include "EntityHandle.hpp"
#include "ComponentPool.hpp"
#include "Graphics/ScreenSpace.hpp"
#include "Graphics/BasicRenderer.hpp"

// Forward declarations.
namespace System
{
    class Window;
}

/*
    Render System
*/

namespace Game
{
    // Forward declarations.
    class EntitySystem;
    class ComponentSystem;

    namespace Components
    {
        class Transform;
        class Render;
    }

    // Render system info structure.
    struct RenderSystemInfo
    {
        RenderSystemInfo();

        System::Window* window;
        Graphics::BasicRenderer* basicRenderer;
        EntitySystem* entitySystem;
        ComponentSystem* componentSystem;
    };

    // Render system class.
    class RenderSystem
    {
    public:
        RenderSystem();
        ~RenderSystem();

        // Initializes the render system.
        bool Initialize(const RenderSystemInfo& info);

        // Draws the scene.
        void Draw();

    private:
        // Type delcarations.
        typedef std::vector<Graphics::Sprite::Info> SpriteInfoList;
        typedef std::vector<Graphics::Sprite::Data> SpriteDataList;
        typedef std::vector<std::size_t>            SpriteSortList;

    private:
        // Finalizes a render component.
        bool FinalizeComponent(EntityHandle entity);

    private:
        // Event receiver.
        Receiver<bool(EntityHandle)> m_entityFinalize;

        // Instance references.
        System::Window*          m_window;
        Graphics::BasicRenderer* m_basicRenderer;

        // Component pools.
        ComponentPool<Components::Transform>* m_transformComponents;
        ComponentPool<Components::Render>*    m_renderComponents;

        // Screen space transform.
        Graphics::ScreenSpace m_screenSpace;

        // Sprite drawing lists.
        SpriteInfoList m_spriteInfo;
        SpriteDataList m_spriteData;
        SpriteSortList m_spriteSort;

        // Initialization state.
        bool m_initialized;
    };
}
