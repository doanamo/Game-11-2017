#include "Precompiled.hpp"
#include "Platform.hpp"
using namespace System;

namespace
{
    // Callback error function.
    void ErrorCallback(int error, const char* description)
    {
        LogWarning() << "GLFW Error: " << description;
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
    Log() << "Initializing platform..." << LogIndent();

    // Check if system platform is already initialized.
    Verify(!m_initialized, "Platform is already initialized!");

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
    LogInfo() << "Success!";

    return m_initialized = true;
}
