#pragma once

#include "Precompiled.hpp"

/*
    Scoped Indent

    Increases logging indent for a duration of a scope.

    void ExampleScopedIndent()
    {
        // Initialize the logging system.
        Logger::Initialize();

        // Using a macro to create an indent.
        Log() << "Initializing nothingness..." << LogIndent();

        // Write a log message.
        Log() << "Success!";
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

// Utility macros.
#define LogIndent() ""; Logger::ScopedIndent LOGGER_INDENT_NAME(__LINE__)(Logger::GetGlobalSink())

#define LOGGER_INDENT_NAME(line) LOGGER_INDENT_STRING(line)
#define LOGGER_INDENT_STRING(line) loggerIndent ## line
