#include "Precompiled.hpp"
#include "Logger.hpp"
#include "Output.hpp"
#include "Sink.hpp"

namespace
{
    // Default logger sink.
    Logger::Sink sink;

    // Default logger outputs.
    Logger::DebuggerOutput debuggerOutput;
    Logger::ConsoleOutput consoleOutput;
    Logger::FileOutput fileOutput;

    // Initialization state.
    bool initialized = false;
}

void Logger::Initialize()
{
    Verify(!initialized, "Default logger sink has been already initialized!");

    // Add the console output.
    sink.AddOutput(&consoleOutput);

    // Add the debugger output.
    sink.AddOutput(&debuggerOutput);

    // Add the file output.
    if(fileOutput.Open("Log.txt"))
    {
        sink.AddOutput(&fileOutput);
    }

    // Set initialized state.
    initialized = true;
}

void Logger::Write(const Logger::Message& message)
{
    Verify(initialized, "Default logger has not been initialized yet!");

    sink.Write(message);
}

int Logger::AdvanceFrameReference()
{
    Verify(initialized, "Default logger has not been initialized yet!");

    return sink.AdvanceFrameReference();
}

Logger::Sink* Logger::GetGlobalSink()
{
    Verify(initialized, "Default logger has not been initialized yet!");

    return &sink;
}
