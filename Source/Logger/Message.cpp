#include "Precompiled.hpp"
#include "Message.hpp"
#include "Sink.hpp"
using namespace Logger;

namespace
{
    // Case insensitive character comparison function for std::search.
    bool CaseInsensitiveCharacterComparison(char a, char b)
    {
        return std::tolower(a) == std::tolower(b);
    };
}

Message::Message() :
    std::ostream(&m_text),
    m_severity(Severity::Info),
    m_source(),
    m_line(0)
{
}

Message::Message(Message&& other) :
    std::ostream(&m_text)
{
    m_severity = other.m_severity;
    other.m_severity = Severity::Info;

    m_text = std::move(other.m_text);
    m_source = std::move(other.m_source);

    m_line = other.m_line;
    other.m_line = 0;
}

Message::~Message()
{
}

Message& Message::SetSeverity(Severity::Type severity)
{
    Verify(Severity::Invalid < severity && severity < Severity::Count, "Severity argument is invalid!");

    m_severity = severity;
    return *this;
}

Message& Message::SetText(const char* text)
{
    Verify(text != nullptr, "Attempting to assign nullptr to a string!");

    m_text.str(text);
    return *this;
}

Message& Message::SetSource(const char* source)
{
    Verify(source != nullptr, "Attempting to assign nullptr to a string!");

    // Assign source string.
    m_source = source;

    // Truncate the source file path.
    if(!m_source.empty())
    {
        // Normalize source path separators.
        std::replace(m_source.begin(), m_source.end(), '\\', '/');

        // Get the source directory path.
        std::string sourceDir = Build::GetSourceDir();

        if(sourceDir.empty())
        {
            // Workaround in case source directory is not specified.
            sourceDir = "Source/";
        }

        // Find and remove base path to source directory.
        auto it = std::search(
            m_source.begin(), m_source.end(),
            sourceDir.begin(), sourceDir.end(),
            CaseInsensitiveCharacterComparison
        );

        if(it != m_source.end())
        {
            m_source.erase(m_source.begin(), it + sourceDir.size());
        }

        // Workaround for the first letter being lower case on Visual C++ compiler.
        // Happenes whenever __FILE__ macro is used inside an inlined function.
        if(!m_source.empty())
        {
            m_source[0] = std::toupper(m_source[0]);
        }
    }

    return *this;
}

Message& Message::SetLine(int line)
{
    Verify(line > 0, "Attempting to set an invalid source line!");

    m_line = line;
    return *this;
}

Severity::Type Message::GetSeverity() const
{
    return m_severity;
}

std::string Message::GetText() const
{
    return m_text.str();
}

std::string Message::GetSource() const
{
    return m_source;
}

int Message::GetLine() const
{
    return m_line;
}

bool Message::IsEmpty() const
{
    return m_text.str().empty();
}

ScopedMessage::ScopedMessage(Sink* sink) :
    m_sink(sink)
{
    Verify(sink != nullptr, "Scoped message needs a valid sink reference!");
}

ScopedMessage::ScopedMessage(ScopedMessage&& other)
{
    m_sink = other.m_sink;
    other.m_sink = nullptr;
}

ScopedMessage::~ScopedMessage()
{
    if(m_sink != nullptr)
    {
        m_sink->Write(*this);
    }
}
