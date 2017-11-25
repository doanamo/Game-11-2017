#include "Precompiled.hpp"
#include "Sink.hpp"
#include "Output.hpp"
#include "Message.hpp"
using namespace Logger;

Sink::Sink() :
    m_context()
{
}

Sink::~Sink()
{
}

void Sink::SetName(std::string name)
{
    m_context.name = name;
}

void Sink::AddOutput(Logger::Output* output)
{
    if(output == nullptr)
        return;

    // Add an output to the list.
    m_outputs.push_back(output);
}

void Sink::RemoveOutput(Logger::Output* output)
{
    if(output == nullptr)
        return;

    // Find and remove an output from the list.
    m_outputs.erase(std::remove(m_outputs.begin(), m_outputs.end(), output), m_outputs.end());
}

void Sink::Write(const Logger::Message& message)
{
    // Write a message to all outputs.
    for(auto output : m_outputs)
    {
        output->Write(message, m_context);
    }

    m_context.messageWritten = true;
}

int Sink::AdvanceFrameReference()
{
    // Advance the frame of reference only if a message has
    // been written since the last time counter was incremented.
    if(m_context.messageWritten)
    {
        m_context.referenceFrame++;
        m_context.messageWritten = false;
    }

    return m_context.referenceFrame;
}

const SinkContext& Sink::GetContext() const
{
    return m_context;
}
