#include "Precompiled.hpp"
#include "RenderSystem.hpp"
#include "EntitySystem.hpp"
#include "ComponentSystem.hpp"
#include "TransformComponent.hpp"
#include "RenderComponent.hpp"
#include "System/Window.hpp"
#include "Graphics/BasicRenderer.hpp"
using namespace Game;

namespace
{
    // Error messages.
    #define LogInitializeError() "Failed to initialize the render system! "

    // Global render scale.
    const glm::vec3 RenderScale(1.0f / 128.0f, 1.0f / 128.0f, 1.0f);
}

RenderSystemInfo::RenderSystemInfo() :
    window(nullptr),
    basicRenderer(nullptr),
    entitySystem(nullptr),
    componentSystem(nullptr)
{
}

RenderSystem::RenderSystem() :
    m_window(nullptr),
    m_basicRenderer(nullptr),
    m_transformComponents(nullptr),
    m_renderComponents(nullptr),
    m_initialized(false)
{
    // Bind event receivers.
    m_entityFinalize.Bind<RenderSystem, &RenderSystem::FinalizeComponent>(this);
}

RenderSystem::~RenderSystem()
{
}

bool RenderSystem::Initialize(const RenderSystemInfo& info)
{
    // Check if instance has been already initialized.
    if(m_initialized)
    {
        Log() << LogInitializeError() << "Instance has been already initialized.";
        return false;
    }

    // Validate arguments.
    if(info.window == nullptr)
    {
        Log() << LogInitializeError() << "Invalid argument - \"info.window\" is null.";
        return false;
    }

    if(info.basicRenderer == nullptr)
    {
        Log() << LogInitializeError() << "Invalid argument - \"info.basicRenderer\" is null.";
        return false;
    }

    if(info.entitySystem == nullptr)
    {
        Log() << LogInitializeError() << "Invalid argument - \"info.entitySystem\" is null.";
        return false;
    }

    if(info.componentSystem == nullptr)
    {
        Log() << LogInitializeError() << "Invalid argument - \"info.componentSystem\" is null.";
        return false;
    }

    // Save instance references.
    m_window = info.window;
    m_basicRenderer = info.basicRenderer;

    SCOPE_GUARD_BEGIN(!m_initialized);
    {
        m_window = nullptr;
        m_basicRenderer = nullptr;
    }
    SCOPE_GUARD_END();

    // Retrieve component pools.
    m_transformComponents = info.componentSystem->GetPool<Components::Transform>();
    m_renderComponents = info.componentSystem->GetPool<Components::Render>();

    SCOPE_GUARD_BEGIN(!m_initialized);
    {
        m_transformComponents = nullptr;
        m_renderComponents = nullptr;
    }
    SCOPE_GUARD_END();

    if(m_transformComponents == nullptr)
    {
        Log() << LogInitializeError() << "Could not get the pool for transform components.";
        return false;
    }

    if(m_renderComponents == nullptr)
    {
        Log() << LogInitializeError() << "Could not get the pool for render components.";
        return false;
    }

    // Set screen space target size.
    m_screenSpace.SetSourceSize(10.0f, 10.0f);

    SCOPE_GUARD_IF(!m_initialized, m_screenSpace = Graphics::ScreenSpace());

    // Allocate initial sprite list memory.
    const int SpriteListSize = 128;
    m_spriteInfo.reserve(SpriteListSize);
    m_spriteData.reserve(SpriteListSize);
    m_spriteSort.reserve(SpriteListSize);

    SCOPE_GUARD_BEGIN(!m_initialized);
    {
        Utility::ClearContainer(m_spriteInfo);
        Utility::ClearContainer(m_spriteData);
        Utility::ClearContainer(m_spriteSort);
    }
    SCOPE_GUARD_END();

    // Subscribe to the entity system.
    if(!m_entityFinalize.Subscribe(info.entitySystem->eventDispatchers.entityFinalize))
    {
        Log() << LogInitializeError() << "Could not subscribe to the entity system.";
        return false;
    }

    SCOPE_GUARD_IF(!m_initialized, m_entityFinalize.Unsubscribe());

    // Success!
    return m_initialized = true;
}

bool RenderSystem::FinalizeComponent(EntityHandle entity)
{
    Verify(m_initialized);

    // Get the transform component.
    auto transformComponent = m_transformComponents->Lookup(entity);
    if(transformComponent == nullptr) return false;

    // Get the render component.
    auto renderComponent = m_renderComponents->Lookup(entity);
    if(renderComponent == nullptr) return false;

    // Set the reference for transform component.
    renderComponent->m_transform = transformComponent;

    return true;
}

void RenderSystem::Draw()
{
    if(!m_initialized)
        return;

    // Get window size.
    int windowWidth = m_window->GetWidth();
    int windowHeight = m_window->GetHeight();

    // Set viewport size.
    glViewport(0, 0, windowWidth, windowHeight);

    // Set screen space source size.
    m_screenSpace.SetTargetSize(windowWidth, windowHeight);

    // Calculate camera view.
    glm::mat4 view = glm::translate(glm::mat4(1.0f), -glm::vec3(m_screenSpace.GetOffset(), 0.0f));
    glm::mat4 transform = m_screenSpace.GetTransform() * view;

    // Clear the backbuffer.
    Graphics::ClearValues clearValues;
    clearValues.color = glm::vec4(0.0f, 0.35f, 0.35f, 1.0f);
    clearValues.depth = 1.0f;

    m_basicRenderer->Clear(clearValues);

    // Iterate over all render components.
    auto componentsBegin = m_renderComponents->Begin();
    auto componentsEnd = m_renderComponents->End();

    for(auto it = componentsBegin; it != componentsEnd; ++it)
    {
        // Get entity components.
        Components::Render* render = &it->second;
        Assert(render != nullptr);

        Components::Transform* transform = render->GetTransform();
        Assert(transform != nullptr);

        // Add sprite to render the list.
        Graphics::Sprite::Info info;
        info.texture = render->GetTexture().get();
        info.transparent = render->IsTransparent();
        info.filter = false;

        Graphics::Sprite::Data data;
        data.transform = glm::translate(data.transform, transform->GetPosition());
        //data.transform = glm::rotate(data.transform, transform->GetRotation(), glm::vec3(0.0f, 0.0f, -1.0f));
        data.transform = glm::scale(data.transform, transform->GetScale() * RenderScale);
        //data.transform = glm::translate(data.transform, glm::vec3(0.0f, 0.0f, 0.0f));
        data.rectangle = render->GetRectangle();
        data.color = render->CalculateColor();

        m_spriteInfo.push_back(info);
        m_spriteData.push_back(data);
    }

    // Define sorting function.
    auto SpriteSort = [&](const int& a, const int& b)
    {
        // Get sprite info and data.
        const auto& spriteInfoA = m_spriteInfo[a];
        const auto& spriteDataA = m_spriteData[a];

        const auto& spriteInfoB = m_spriteInfo[b];
        const auto& spriteDataB = m_spriteData[b];

        // Sort by transparency (opaque first, transparent second).
        if(spriteInfoA.transparent < spriteInfoB.transparent)
            return true;

        if(spriteInfoA.transparent == spriteInfoB.transparent)
        {
            if(spriteInfoA.transparent)
            {
                // Sort transparent by depth (back to front).
                if(spriteDataA.transform[3][2] < spriteDataB.transform[3][2])
                    return true;

                if(spriteDataA.transform[3][2] == spriteDataB.transform[3][2])
                {
                    // Sort by the y position.
                    if(spriteDataA.transform[3][1] > spriteDataB.transform[3][1])
                        return true;

                    if(spriteDataA.transform[3][1] == spriteDataB.transform[3][1])
                    {
                        // Sort by texture.
                        if(spriteInfoA.texture < spriteInfoB.texture)
                            return true;
                    }
                }
            }
            else
            {
                // Sort opaque by depth (front to back).
                if(spriteDataA.transform[3][2] > spriteDataB.transform[3][2])
                    return true;

                if(spriteDataA.transform[3][2] == spriteDataB.transform[3][2])
                {
                    // Sort by texture.
                    if(spriteInfoA.texture < spriteInfoB.texture)
                        return true;
                }
            }
        }

        return false;
    };

    // Create sort permutation.
    Assert(m_spriteInfo.size() == m_spriteData.size());

    m_spriteSort.resize(m_spriteInfo.size());
    std::iota(m_spriteSort.begin(), m_spriteSort.end(), 0);
    std::sort(m_spriteSort.begin(), m_spriteSort.end(), SpriteSort);

    // Sort sprite lists.
    Utility::Reorder(m_spriteInfo, m_spriteSort);
    Utility::Reorder(m_spriteData, m_spriteSort);

    // Draw sprites.
    m_basicRenderer->DrawSprites(m_spriteInfo, m_spriteData, transform);

    // Clear the sprite list.
    m_spriteInfo.clear();
    m_spriteData.clear();
}
