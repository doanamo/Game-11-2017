#include "Precompiled.hpp"
#include "System/Platform.hpp"
#include "System/Config.hpp"
#include "System/Timer.hpp"
#include "System/Window.hpp"
#include "System/InputState.hpp"
#include "System/ResourceManager.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/BasicRenderer.hpp"
#include "Scripting/State.hpp"
#include "Scripting/Reference.hpp"
#include "Scripting/Helpers.hpp"
#include "Game/EntitySystem.hpp"
#include "Game/ComponentSystem.hpp"
#include "Game/TransformComponent.hpp"
#include "Game/ScriptSystem.hpp"
#include "Game/ScriptBindings.hpp"
#include "Game/ScriptComponent.hpp"
#include "Game/RenderSystem.hpp"
#include "Game/RenderComponent.hpp"

namespace
{
    // Log error messages.
    #define LogFatalError() "Fatal error has been encountered! "
}

int main(int argc, char* argv[])
{
    // Initialize core systems.
    Logger::Initialize();
    Debug::Initialize();
    Build::Initialize();

    // Initialize the platform.
    System::Platform platform;
    if(!platform.Initialize())
    {
        Log() << LogFatalError() << "Could not initialize the platform.";
        return -1;
    }

    // Create a scripting state.
    Scripting::State scriptingState;
    if(!scriptingState.Load("Data/Scripts/Main.lua"))
    {
        Log() << LogFatalError() << "Could not load the main script entry.";
        return -1;
    }

    // Create a config.
    System::Config config;
    config.Load("Game.cfg");

    // Create a timer.
    System::Timer timer;
    timer.SetMaxFrameDelta(1.0f);

    // Create a window.
    System::WindowInfo windowInfo;
    windowInfo.title = config.GetParameter<std::string>("Window.Title", "Game");
    windowInfo.width = config.GetParameter<int>("Window.Width", 1024);
    windowInfo.height = config.GetParameter<int>("Window.Height", 576);
    windowInfo.vsync = config.GetParameter<bool>("Window.Vsync", true);

    System::Window window;
    if(!window.Open(windowInfo))
    {
        Log() << LogFatalError() << "Could not open a window.";
        return -1;
    }

    // Add build information to the window's title.
    std::string windowTitle = window.GetTitle();
    windowTitle += " - CL " + Build::GetChangeList();
    windowTitle += ", " + Build::GetCommitHash();
    windowTitle += ", " + Build::GetBranchName();
    windowTitle += " - " + Build::GetCommitDate();
    window.SetTitle(windowTitle);

    // Create an input state.
    System::InputState inputState;
    if(!inputState.Subscribe(window))
    {
        Log() << LogFatalError() << "Could not subscribe an input state.";
        return -1;
    }

    // Create a resource manager.
    System::ResourceManager resourceManager;

    // Create a basic renderer.
    Graphics::BasicRendererInfo basicRendererInfo;
    basicRendererInfo.resourceManager = &resourceManager;
    basicRendererInfo.spriteBatchSize = 128;

    Graphics::BasicRenderer basicRenderer;
    if(!basicRenderer.Initialize(basicRendererInfo))
    {
        Log() << LogFatalError() << "Could not initialize a basic renderer.";
        return -1;
    }

    // Create an entity system.
    Game::EntitySystem entitySystem;

    // Create a component system.
    Game::ComponentSystem componentSystem;
    if(!componentSystem.Subscribe(entitySystem))
    {
        Log() << LogFatalError() << "Could not subscribe a component system.";
        return -1;
    }

    // Create a script system.
    Game::ScriptSystemInfo scriptSystemInfo;
    scriptSystemInfo.scriptingState = &scriptingState;
    scriptSystemInfo.entitySystem = &entitySystem;
    scriptSystemInfo.componentSystem = &componentSystem;

    Game::ScriptSystem scriptSystem;
    if(!scriptSystem.Initialize(scriptSystemInfo))
    {
        Log() << LogFatalError() << "Could not initialize a script system.";
        return -1;
    }

    Game::ScriptBindings::References scriptBindingsReferences;
    scriptBindingsReferences.inputState = &inputState;
    scriptBindingsReferences.componentSystem = &componentSystem;

    if(!Game::ScriptBindings::Register(&scriptingState, scriptBindingsReferences))
    {
        Log() << LogFatalError() << "Could not register script bindings.";
        return -1;
    }

    // Create a render system.
    Game::RenderSystemInfo renderSystemInfo;
    renderSystemInfo.window = &window;
    renderSystemInfo.basicRenderer = &basicRenderer;
    renderSystemInfo.entitySystem = &entitySystem;
    renderSystemInfo.componentSystem = &componentSystem;

    Game::RenderSystem renderSystem;
    if(!renderSystem.Initialize(renderSystemInfo))
    {
        Log() << LogFatalError() << "Could not initialize a render system.";
        return -1;
    }
    
    // Create an example entity.
    Game::EntityHandle entity = entitySystem.CreateEntity();

    {
        using namespace Game::Components;

        auto* transform = componentSystem.Create<Transform>(entity);
        transform->SetPosition(0.0f, 0.0f, 0.0f);

        auto* script = componentSystem.Create<Script>(entity);
        script->AddScript(resourceManager.Load<Scripting::Reference>("Data/Scripts/Player.lua", &scriptingState));

        auto* render = componentSystem.Create<Render>(entity);
        render->SetTexture(resourceManager.Load<Graphics::Texture>("Data/Textures/ColorCheckerboard.png"));
        render->SetRectangleFromTexture();
    }

    // Main loop.
    while(window.IsOpen())
    {
        // Advance logger's frame of reference.
        Logger::AdvanceFrameReference();

        // Calculate frame delta time.
        float timeDelta = timer.CalculateFrameDelta();

        // Prepare input state for incoming events.
        inputState.Prepare();

        // Process window events.
        window.ProcessEvents();

        // Process entity commands.
        entitySystem.ProcessCommands();

        // Update the script system.
        scriptSystem.Update(timeDelta);

        // Draw the scene.
        renderSystem.Draw();

        // Present to the window.
        window.Present();

        // Clean the scripting state.
        scriptingState.CleanStack();
        scriptingState.CollectGarbage(0.002f);

        // Release unused resources.
        resourceManager.ReleaseUnused();

        // Tick the timer.
        timer.Tick();
    }

    return 0;
}
