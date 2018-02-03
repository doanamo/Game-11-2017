#pragma once

#include "Precompiled.hpp"

/*
    Default Logging Format
*/

namespace Logger
{
    // Forward declarations.
    class Message;
    struct SinkContext;

    // Default format class.
    class DefaultFormat
    {
    public:
        static std::string ComposeSessionStart();
        static std::string ComposeMessage(const Message& message, const SinkContext& context);
        static std::string ComposeSessionEnd();
    };
}