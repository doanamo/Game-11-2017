#include "Precompiled.hpp"
#include "Logger.hpp"
#include "Output.hpp"
#include "Sink.hpp"

namespace
{
    // Logger sink.
    Logger::Sink sink;

    // Logger outputs.
    Logger::DebuggerOutput debuggerOutput;
    Logger::ConsoleOutput consoleOutput;
    Logger::FileOutput fileOutput;

    // Initialization state.
    bool initialized = false;
}

void Logger::Initialize()
{
    if(initialized)
        return;

    // Add the debugger output.
    sink.AddOutput(&debuggerOutput);

    // Add the console output.
    sink.AddOutput(&consoleOutput);

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
    sink.Write(message);
}

int Logger::AdvanceFrameReference()
{
    return sink.AdvanceFrameReference();
}

Logger::Sink* Logger::GetGlobalSink()
{
    return &sink;
}
