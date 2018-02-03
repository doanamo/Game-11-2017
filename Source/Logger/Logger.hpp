#pragma once

#include "Precompiled.hpp"
#include "Message.hpp"
#include "Sink.hpp"

/*
    Logger

    Writes log messages to multiple outputs for debugging purposes.
    
    Example usage:
        Logger::Initialize();
        Log() << "Hello world!";
*/

namespace Logger
{
    // Forward declarations.
    class Sink;

    // Initializes the logger.
    void Initialize();

    // Writes to the global logger sink.
    void Write(const Logger::Message& message);

    // Advance frame of reference.
    int AdvanceFrameReference();
    
    // Gets the global logger sink.
    Sink* GetGlobalSink();
}

// Utility macros.
#define LogInfo()    Log().SetSeverity(Logger::Severity::Info)
#define LogDebug()   Log().SetSeverity(Logger::Severity::Debug)
#define LogWarning() Log().SetSeverity(Logger::Severity::Warning)
#define LogError()   Log().SetSeverity(Logger::Severity::Error)

#ifndef NDEBUG
    #define Log() Logger::ScopedMessage(Logger::GetGlobalSink()).SetSource(__FILE__).SetLine(__LINE__)
#else
    #define Log() Logger::ScopedMessage(Logger::GetGlobalSink())
#endif
