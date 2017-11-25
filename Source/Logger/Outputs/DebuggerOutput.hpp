#pragma once

#include "Precompiled.hpp"
#include "Logger/Output.hpp"

/*
    Logger Debugger Output

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
