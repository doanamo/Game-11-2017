#pragma once

#include "Precompiled.hpp"
#include "Logger/Output.hpp"

/*
    Logger Console Output

    Writes log messsages to the console's window.
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
