#pragma once

#include "Precompiled.hpp"

/*
    Logger Sink

    Writes messages to multiple outputs.
*/

namespace Logger
{
    // Forward declarations.
    class Output;
    class Message;

    // Sink context structure.
    struct SinkContext
    {
        SinkContext() :
            referenceFrame(0),
            messageWritten(false)
        {
        }

        std::string name;
        int referenceFrame;
        int messageIndent;
        bool messageWritten;
    };

    // Sink class.
    class Sink : private NonCopyable
    {
    public:
        // Type declarations.
        typedef std::vector<Logger::Output*> OutputList;

    public:
        Sink();
        ~Sink();

        // Sets the output name.
        void SetName(std::string name);

        // Adds an output.
        void AddOutput(Logger::Output* output);

        // Removes an output.
        void RemoveOutput(Logger::Output* output);

        // Writes a log message.
        void Write(const Logger::Message& message);

        // Advance frame of reference.
        int AdvanceFrameReference();

        // Increase current message indent.
        void IncreaseIndent();

        // Decrease current message indent.
        void DecreaseIndent();

        // Gets the context.
        const SinkContext& GetContext() const;

    private:
        // Sink's context.
        SinkContext m_context;

        // List of outputs.
        OutputList m_outputs;
    };
}

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
    // Scoped indent class.
    class ScopedIndent
    {
    public:
        ScopedIndent(Sink* sink);
        ~ScopedIndent();

    private:
        // Logger sink.
        Sink * m_sink;
    };
}

// Utility macros.
#define LogIndent() ""; Logger::ScopedIndent LOGGER_INDENT_NAME(__LINE__)(Logger::GetGlobalSink())

#define LOGGER_INDENT_NAME(line) LOGGER_INDENT_STRING(line)
#define LOGGER_INDENT_STRING(line) loggerIndent ## line
