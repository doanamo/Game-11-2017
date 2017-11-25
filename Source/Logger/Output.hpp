#pragma once

#include "Precompiled.hpp"

/*
    Logger Output
    
    Base class for output implementations.
*/

namespace Logger
{
    // Forward declarations.
    class Message;
    struct SinkContext;

    // Output interface.
    class Output : private NonCopyable
    {
    public:
        // Writes a message to an output.
        virtual void Write(const Logger::Message& message, const Logger::SinkContext& context) = 0;
    };
}
