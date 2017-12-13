#include "Precompiled.hpp"
#include "System/Context.hpp"
#include "System/Config.hpp"
#include "System/Timer.hpp"
#include "System/Window.hpp"
#include "System/InputState.hpp"
#include "System/ResourceManager.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/VertexInput.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/Sampler.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/ScreenSpace.hpp"
#include "Graphics/BasicRenderer.hpp"

namespace
{
    // Log error messages.
    #define LogFatalError() "Fatal error has been encountered! "
}

int main(int argc, char* argv[])
{
    // Initialize core systems.
    Build::Initialize();
    Debug::Initialize();
    Logger::Initialize();

    // Initialize platform context.
    System::Context context;
    if(!context.Initialize())
    {
        Log() << LogFatalError() << "Could not initialize the context.";
        return -1;
    }

    // Create a config.
    System::Config config;
    config.LoadFromFile("Game.cfg");

    // Create a timer.
    System::Timer timer;

    if(!timer.IsFrequencyValid())
    {
        Log() << LogFatalError() << "Could not create a valid a timer.";
        return -1;
    }

    timer.SetMaxFrameDelta(1.0f);

    // Create a window.
    System::WindowInfo windowInfo;
    windowInfo.name = "Game";
    windowInfo.width = config.GetParameter<int>("Window.Width", 1024);
    windowInfo.height = config.GetParameter<int>("Window.Height", 576);
    windowInfo.vsync = config.GetParameter<bool>("Window.Vsync", true);

    System::Window window;
    if(!window.Open(windowInfo))
    {
        Log() << LogFatalError() << "Could not open a window.";
        return -1;
    }

    // Create an input state.
    System::InputState inputState;
    if(!inputState.Subscribe(window))
    {
        Log() << LogFatalError() << "Could not subscribe an input state.";
        return -1;
    }

    // Create a resource manager.
    System::ResourceManager resourceManager;

    // Create screen space coordinates.
    Graphics::ScreenSpace screenSpace;
    screenSpace.SetSourceSize(128.0f, 128.0f);

    // Create a basic renderer.
    Graphics::BasicRenderer basicRenderer;
    if(!basicRenderer.Initialize(resourceManager))
    {
        Log() << LogFatalError() << "Could not initialize a basic renderer.";
        return -1;
    }

    // Create an example sprite.
    auto texture = resourceManager.Load<Graphics::Texture>("Data/Textures/ColorCheckerboard.png");

    Graphics::BasicRenderer::Sprite sprite;
    sprite.data.transform = glm::mat4(1.0f);
    sprite.data.rectangle = glm::vec4(0.0f, 0.0f, 128.0f, 128.0f);
    sprite.data.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    sprite.info.texture = texture.get();
    sprite.info.transparent = false;
    sprite.info.filter = false;

    // Main loop.
    while(window.IsOpen())
    {
        // Advance logger's frame of reference.
        Logger::AdvanceFrameReference();

        // Calculate frame delta time.
        float frameDelta = timer.CalculateFrameDelta();

        // Update input state.
        inputState.Update();

        // Process window evenets.
        window.ProcessEvents();

        // Update screen space coordinates.
        glViewport(0, 0, window.GetWidth(), window.GetHeight());
        screenSpace.SetTargetSize(window.GetWidth(), window.GetHeight());

        // Clear the framebuffer.
        basicRenderer.SetClearDepth(1.0f);
        basicRenderer.SetClearColor(glm::vec4(0.0f, 0.35f, 0.35f, 1.0f));
        basicRenderer.Clear(Graphics::ClearFlags::Depth | Graphics::ClearFlags::Color);

        // Draw the scene.
        basicRenderer.DrawSprite(sprite, screenSpace.GetTransform());

        // Present to the window.
        window.Present();

        // Tick the timer.
        timer.Tick();
    }

    return 0;
}
