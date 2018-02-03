#include "Precompiled.hpp"
#include "Platform.hpp"
using namespace System;

namespace
{
    // Callback error function.
    void ErrorCallback(int error, const char* description)
    {
        Log() << "GLFW Error: " << description;
    }
}

Platform::Platform() :
    m_initialized(false)
{
}

Platform::~Platform()
{
    // Release GLFW library.
    if(m_initialized)
    {
        glfwTerminate();
    }
}

bool Platform::Initialize()
{
    Log() << "Initializing system platform..." << LogIndent();

    // Check if the instance has been initialized already.
    if(m_initialized)
    {
        LogError() << "Instance is already initialized!";
        return false;
    }

    // Set a callback function for future GLFW errors.
    glfwSetErrorCallback(ErrorCallback);

    // Initialize GLFW library.
    if(!glfwInit())
    {
        LogError() << "Could not initialize GLFW library!";
        return false;
    }

    // Write GLFW details to log.
    int major, minor, revision;
    glfwGetVersion(&major, &minor, &revision);

    LogInfo() << "Using GLFW " << major << "." << minor << "." << revision << " library.";

    // Success!
    Log() << "Success!";

    return m_initialized = true;
}
