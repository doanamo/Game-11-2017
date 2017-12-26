#pragma once

#include "Precompiled.hpp"

/*
    Platform

    Main platform context that should be initialized
    first before other system classes are used.
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
