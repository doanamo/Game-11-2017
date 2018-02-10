#pragma once

#include "Precompiled.hpp"
#include "Message.hpp"
#include "Sink.hpp"

/*
    Logger

    Writes log messages to multiple outputs for debugging purposes.
    
    void ExampleLogger()
    {
        // Initialize the default logger.
        Logger::Initialize();

        // Write a log message.
        Log() << "Hello world!";

        // Write log messages of different severity.
        LogInfo() << "Some diagnostic info.";
        LogDebug() << "Some debug output.";
        LogWarning() << "Some warning message.";
        LogError() << "Some error message.";

        // Create an indent until the end of a scope.
        {
            Log() << "Non indented message..." << LogIndent();
            Log() << "Indented message!";
        }

        Log() << "Non indented message.";
    }
*/

namespace Logger
{
    // Initializes the default logger sink.
    void Initialize();

    // Writes to the global logger sink.
    void Write(const Message& message);

    // Advances the frame of reference.
    // This helps distinguish messages from different frames.
    int AdvanceFrameReference();
    
    // Gets the global logger sink.
    Sink* GetGlobalSink();
}

// Utility macros.
#ifndef NDEBUG
    #define Log() Logger::ScopedMessage(Logger::GetGlobalSink()).SetSource(__FILE__).SetLine(__LINE__)
#else
    #define Log() Logger::ScopedMessage(Logger::GetGlobalSink())
#endif

#define LogInfo()    Log().SetSeverity(Logger::Severity::Info)
#define LogDebug()   Log().SetSeverity(Logger::Severity::Debug)
#define LogWarning() Log().SetSeverity(Logger::Severity::Warning)
#define LogError()   Log().SetSeverity(Logger::Severity::Error)

#define LogIndent() ""; Logger::ScopedIndent loggerIndent ## line(Logger::GetGlobalSink())
