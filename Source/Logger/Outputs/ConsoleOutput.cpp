#include "Precompiled.hpp"
#include "ConsoleOutput.hpp"
#include "Logger/Sink.hpp"
using namespace Logger;

ConsoleOutput::ConsoleOutput()
{
}

ConsoleOutput::~ConsoleOutput()
{
}

void ConsoleOutput::Write(const Message& message, const SinkContext& context)
{
    // Write message prefix.
    time_t timeData = time(nullptr);
    tm* timeInfo = localtime(&timeData);

    std::cout << "[";
    std::cout << std::setfill('0');
    std::cout << std::setw(2) << timeInfo->tm_hour << ":";
    std::cout << std::setw(2) << timeInfo->tm_min  << ":";
    std::cout << std::setw(2) << timeInfo->tm_sec;
    std::cout << std::setfill(' ') << std::setw(0);
    std::cout << "]";

    // Write frame reference.
    std::cout << "[";
    std::cout << std::setfill('0') << std::setw(3);
    std::cout << context.referenceFrame % 1000;
    std::cout << std::setfill(' ') << std::setw(0);
    std::cout << "] ";

    // Write message text.
    std::cout << message.GetText();

    // Write message source.
    if(!message.GetSource().empty())
    {
        std::cout << " {";
        std::cout << message.GetSource();

        if(message.GetLine() != 0)
        {
            std::cout << ":";
            std::cout << message.GetLine();
        }

        std::cout << "}";
    }

    // Write message suffix.
    std::cout << "\n";

    // Flush the file stream.
    std::cout.flush();
}
