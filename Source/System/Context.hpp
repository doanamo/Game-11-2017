#pragma once

#include "Precompiled.hpp"

/*
    Context

    Main platform context that has to be initialized
    first before other system classes can be used.
*/

namespace System
{
    // Context class.
    class Context
    {
    public:
        Context();
        ~Context();

        // Restores class instance to its original state.
        void Cleanup();

        // Initializes the platform context.
        bool Initialize();

    private:
        // Initialization state.
        bool m_initialized;
    };
}
