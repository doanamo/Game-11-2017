#include "Precompiled.hpp"
#include "Platform.hpp"
using namespace System;

namespace
{
    // Log messages.
    #define LogInitializeError() "Failed to initialize the platform! "

    // Callback functions.
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
    // Check if instance has been already initialized.
    if(m_initialized)
    {
        Log() << LogInitializeError() << "Instance is already initialized.";
        return false;
    }

    // Set a callback function for future GLFW errors.
    glfwSetErrorCallback(ErrorCallback);

    // Initialize GLFW library.
    if(!glfwInit())
    {
        Log() << LogInitializeError() << "Could not initialize GLFW library.";
        return false;
    }

    // Success!
    int major, minor, revision;
    glfwGetVersion(&major, &minor, &revision);

    Log() << "Initialized the platform with GLFW " << major << "." << minor << "." << revision << " library.";

    return m_initialized = true;
}
