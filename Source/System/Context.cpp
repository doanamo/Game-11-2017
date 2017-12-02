#include "Precompiled.hpp"
#include "Context.hpp"
using namespace System;

namespace
{
    // Callback functions.
    void ErrorCallback(int error, const char* description)
    {
        Log() << "GLFW Error: " << description;
    }
}

Context::Context() :
    m_initialized(false)
{
}

Context::~Context()
{
    this->Cleanup();
}

void Context::Cleanup()
{
    if(m_initialized)
    {
        glfwTerminate();
        m_initialized = false;
    }
}

bool Context::Initialize()
{
    if(m_initialized)
        return true;

    // Set a callback function for future GLFW errors.
    glfwSetErrorCallback(ErrorCallback);

    // Initialize GLFW library.
    if(!glfwInit())
    {
        Log() << "Failed to initialize GLFW library for host platform!";
        return false;
    }

    // Success!
    int major, minor, revision;
    glfwGetVersion(&major, &minor, &revision);

    Log() << "Initialized platform context for GLFW " << major << "." << minor << "." << revision << " host.";

    return m_initialized = true;
}
