#include "Precompiled.hpp"
#include "System/Context.hpp"
#include "System/Config.hpp"
#include "System/Timer.hpp"
#include "System/Window.hpp"

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

    // Instantiate a config instance.
    System::Config config;

    if(!config.LoadFromFile("Game.cfg"))
    {
        Log() << LogFatalError() << "Could not load a config file.";
        return -1;
    }

    // Preare window initialization structure.
    System::WindowInfo windowInfo;
    windowInfo.name = "Game";
    windowInfo.width = config.GetParameter<int>("Window.Width", 1024);
    windowInfo.height = config.GetParameter<int>("Window.Height", 576);
    windowInfo.vsync = config.GetParameter<bool>("Window.Vsync", true);

    // Instantiate a window instance.
    System::Window window;

    if(!window.Open(windowInfo))
    {
        Log() << LogFatalError() << "Could not open a window.";
        return -1;
    }

    // Instantiate a timer instance.
    System::Timer timer;

    if(!timer.IsValid())
    {
        Log() << LogFatalError() << "Could not create a valid a timer.";
        return -1;
    }

    timer.SetMaxFrameDelta(1.0f);

    // Main loop.
    while(window.IsOpen())
    {
        // Calculate frame delta time.
        float frameDelta = timer.CalculateFrameDelta();

        // Process window evenets.
        window.ProcessEvents();

        // Clear the framebuffer.
        glClearColor(0.0f, 0.35f, 0.35f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Present to the window.
        window.Present();

        // Tick the timer.
        timer.Tick();
    }

    return 0;
}
