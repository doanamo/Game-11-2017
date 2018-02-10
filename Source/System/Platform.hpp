#pragma once

#include "Precompiled.hpp"

/*
    System Platform

    Main platform context that must be initialized
    first before other system classes are used.

    This class encapsulated initialization routines
    that just have to be called at the very beginning
    of the main() function.

    void ExmapleSystemPlatform()
    {
        // Initialize the platform instance.
        System::Platform platform;
        if(!platform.Initialize())
            return;

        // Now all the system classes can be used.
        System::Window window;
        System::Timer timer;
    }
*/

namespace System
{
    // Platform class.
    class Platform
    {
    public:
        Platform();
        ~Platform();

        // Initializes the platform context.
        bool Initialize();

    private:
        // Initialization state.
        bool m_initialized;
    };
}
