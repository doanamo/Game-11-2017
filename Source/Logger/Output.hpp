#pragma once

#include "Precompiled.hpp"

/*
    Base Output
    
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

/*
    File Output

    Writes log messages to a file.
*/

namespace Logger
{
    class FileOutput : public Output
    {
    public:
        FileOutput();
        ~FileOutput();

        // Open the file output.
        bool Open(std::string filename);

        // Writes a message to the file.
        void Write(const Message& message, const SinkContext& context);

    private:
        // File output stream.
        std::ofstream m_file;
    };
}

/*
    Console Output

    Writes log messsages to the application's console window.
*/

namespace Logger
{
    class ConsoleOutput : public Output
    {
    public:
        ConsoleOutput();
        ~ConsoleOutput();

        // Writes a message to the console window.
        void Write(const Message& message, const SinkContext& context);
    };
}

/*
    Debugger Output

    Writes log messages to the debugger's window.
*/

namespace Logger
{
    class DebuggerOutput : public Output
    {
    public:
        DebuggerOutput();
        ~DebuggerOutput();

        // Writes a message to the debugger window.
        void Write(const Message& message, const SinkContext& context);

    private:
        // Reusable string stream.
        std::stringstream m_stream;
    };
}

