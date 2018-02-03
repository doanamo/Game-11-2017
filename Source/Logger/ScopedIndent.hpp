#pragma once

#include "Precompiled.hpp"

/*
    Scoped Indent

    Increases logging indent for a duration of a scope.

    void ExampleScopedIndent()
    {
        // Initialize the logging system.
        Logger::Initialize();

        // Create a scoped indent instance.
        Logger::ScopedIndent indent(Logger::GetGlobalSink());

        // Write a log message.
        Log() << "Indented message!";
    }
    
*/

namespace Logger
{
    // Forward declaration.
    class Sink;

    // Scoped indent class.
    class ScopedIndent
    {
    public:
        ScopedIndent(Sink* sink);
        ~ScopedIndent();

    private:
        // Logger sink.
        Sink* m_sink;
    };
}
