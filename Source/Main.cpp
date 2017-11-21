#include "Precompiled.hpp"
#include "System/Config.hpp"
#include "System/Window.hpp"

int main(int argc, char* argv[])
{
    Build::Initialize();
    Debug::Initialize();
    Logger::Initialize();

    // Instantiate a config instance.
    System::Config config;
    
    if(!config.LoadFromFile("Game.cfg"))
    {
        Log() << "Fatal error encountered! There was an error loading a config file.";
        return -1;
    }

    // Preare window initialization structure.
    System::WindowInfo windowInfo;
    windowInfo.name = "Game";
    windowInfo.width = config.GetParameter<int>("Window.Width", 1024);
    windowInfo.height = config.GetParameter<int>("Window.Height", 576);
    windowInfo.vsync = config.GetParameter<bool>("Window.Vsync", false);

    // Instantiate a window instance.
    System::Window window;

    if(!window.Initialize(windowInfo))
    {
        Log() << "Fatal error encountered! Could not create a window.";
        return -1;
    }

    // Main loop processing.
    while(window.IsOpen())
    {
        window.ProcessEvents();

        window.Present();
    }

    return 0;
}
