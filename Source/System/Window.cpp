#include "Precompiled.hpp"
#include "Window.hpp"
#include "Config.hpp"
using namespace System;

namespace
{
    // Log message strings.
    #define LogOpenError() "Failed to open a window! "

    // Window callbacks.
    void MoveCallback(GLFWwindow* window, int x, int y)
    {
        Assert(window != nullptr);

        // Get the window instance.
        auto instance = reinterpret_cast<System::Window*>(glfwGetWindowUserPointer(window));
        Assert(instance != nullptr);

        // Send an event.
        Window::Events::Move eventData;
        eventData.x = x;
        eventData.y = y;

        instance->events.move(eventData);
    }

    void ResizeCallback(GLFWwindow* window, int width, int height)
    {
        Assert(window != nullptr);

        // Get the window instance.
        auto instance = reinterpret_cast<System::Window*>(glfwGetWindowUserPointer(window));
        Assert(instance != nullptr);

        // Send an event.
        Window::Events::Resize eventData;
        eventData.width = width;
        eventData.height = height;

        instance->events.resize(eventData);
    }

    void FocusCallback(GLFWwindow* window, int focused)
    {
        Assert(window != nullptr);

        // Get the window instance.
        auto instance = reinterpret_cast<System::Window*>(glfwGetWindowUserPointer(window));
        Assert(instance != nullptr);

        // Send an event.
        Window::Events::Focus eventData;
        eventData.focused = focused > 0;

        instance->events.focus(eventData);
    }

    void CloseCallback(GLFWwindow* window)
    {
        Assert(window != nullptr);

        // Get the window instance.
        auto instance = reinterpret_cast<System::Window*>(glfwGetWindowUserPointer(window));
        Assert(instance != nullptr);

        // Send an event.
        Window::Events::Close eventData;

        instance->events.close(eventData);
    }

    void KeyboardKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        Assert(window != nullptr);

        // Get the window instance.
        auto instance = reinterpret_cast<System::Window*>(glfwGetWindowUserPointer(window));
        Assert(instance != nullptr);

        // Send an event.
        Window::Events::KeyboardKey eventData;
        eventData.key = key;
        eventData.scancode = scancode;
        eventData.action = action;
        eventData.mods = mods;

        instance->events.keyboardKey(eventData);
    }

    void TextInputCallback(GLFWwindow* window, unsigned int character)
    {
        Assert(window != nullptr);

        // Get the window instance.
        auto instance = reinterpret_cast<System::Window*>(glfwGetWindowUserPointer(window));
        Assert(instance != nullptr);

        // Send an event.
        Window::Events::TextInput eventData;
        eventData.character = character;

        instance->events.textInput(eventData);
    }

    void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {
        Assert(window != nullptr);

        // Get the window instance.
        auto instance = reinterpret_cast<System::Window*>(glfwGetWindowUserPointer(window));
        Assert(instance != nullptr);

        // Send an event.
        Window::Events::MouseButton eventData;
        eventData.button = button;
        eventData.action = action;
        eventData.mods = mods;

        instance->events.mouseButton(eventData);
    }

    void MouseScrollCallback(GLFWwindow* window, double offsetx, double offsety)
    {
        Assert(window != nullptr);

        // Get the window instance.
        auto instance = reinterpret_cast<System::Window*>(glfwGetWindowUserPointer(window));
        Assert(instance != nullptr);

        // Send an event.
        Window::Events::MouseScroll eventData;
        eventData.offset = offsety;

        instance->events.mouseScroll(eventData);
    }

    void CursorPositionCallback(GLFWwindow* window, double x, double y)
    {
        Assert(window != nullptr);

        // Get the window instance.
        auto instance = reinterpret_cast<System::Window*>(glfwGetWindowUserPointer(window));
        Assert(instance != nullptr);

        // Send an event.
        Window::Events::CursorPosition eventData;
        eventData.x = x;
        eventData.y = y;

        instance->events.cursorPosition(eventData);
    }

    void CursorEnterCallback(GLFWwindow* window, int entered)
    {
        Assert(window != nullptr);

        // Get the window's instance.
        auto instance = reinterpret_cast<System::Window*>(glfwGetWindowUserPointer(window));
        Assert(instance != nullptr);

        // Send an event.
        Window::Events::CursorEnter eventData;
        eventData.entered = entered != 0;

        instance->events.cursorEnter(eventData);
    }
}

WindowInfo::WindowInfo() :
    name("Window"),
    width(1024),
    height(576),
    vsync(true)
{
}

Window::Window() :
    m_window(nullptr)
{
}

Window::~Window()
{
    this->DestroyWindow();
}

void Window::Cleanup()
{
    this->DestroyWindow();
    this->ResetDispatchers();
}

void Window::DestroyWindow()
{
    // Destroy the window.
    if(m_window != nullptr)
    {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
}

void Window::ResetDispatchers()
{
    // Cleanup event dispatchers.
    events.move.Cleanup();
    events.resize.Cleanup();
    events.focus.Cleanup();
    events.close.Cleanup();
    events.keyboardKey.Cleanup();
    events.textInput.Cleanup();
    events.mouseButton.Cleanup();
    events.mouseScroll.Cleanup();
    events.cursorPosition.Cleanup();
    events.cursorEnter.Cleanup();
}

bool Window::Open(const WindowInfo& info)
{
    // Check if the window is already open.
    if(m_window != nullptr)
    {
        Log() << LogOpenError() << "Window instance is already open.";
        return false;
    }

    // Setup a cleanup guard variable.
    bool initialized = false;

    // Setup window hints.
    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create the window.
    SCOPE_GUARD_IF(!initialized, this->DestroyWindow());

    m_window = glfwCreateWindow(info.width, info.height, info.name.c_str(), nullptr, nullptr);

    if(m_window == nullptr)
    {
        Log() << LogOpenError() << "Could not create the window.";
        return false;
    }

    // Set window user data.
    glfwSetWindowUserPointer(m_window, this);

    // Add event callbacks.
    glfwSetWindowPosCallback(m_window, MoveCallback);
    glfwSetFramebufferSizeCallback(m_window, ResizeCallback);
    glfwSetWindowFocusCallback(m_window, FocusCallback);
    glfwSetWindowCloseCallback(m_window, CloseCallback);
    glfwSetKeyCallback(m_window, KeyboardKeyCallback);
    glfwSetCharCallback(m_window, TextInputCallback);
    glfwSetMouseButtonCallback(m_window, MouseButtonCallback);
    glfwSetScrollCallback(m_window, MouseScrollCallback);
    glfwSetCursorPosCallback(m_window, CursorPositionCallback);
    glfwSetCursorEnterCallback(m_window, CursorEnterCallback);

    // Make window context current.
    glfwMakeContextCurrent(m_window);

    // Set the swap interval.
    glfwSwapInterval((int)info.vsync);

    // Initialize GLEW library for the current context.
    GLenum error = glewInit();

    if(error != GLEW_OK)
    {
        Log() << "GLEW Error: " << glewGetErrorString(error);
        Log() << LogOpenError() << "Could not initialize GLEW library.";
        return false;
    }

    Assert(glGetError() == GL_NO_ERROR, "OpenGL error occurred during context initialization!");

    // Check created OpenGL context.
    int glMajor = glfwGetWindowAttrib(m_window, GLFW_CONTEXT_VERSION_MAJOR);
    int glMinor = glfwGetWindowAttrib(m_window, GLFW_CONTEXT_VERSION_MINOR);

    Log() << "Created OpenGL " << glMajor << "." << glMinor << " context.";

    // Log created window info.
    int windowWidth, windowHeight;
    glfwGetFramebufferSize(m_window, &windowWidth, &windowHeight);
    Log() << "Created a window with " << windowWidth << "x" << windowHeight << " size.";

    // Success!
    return initialized = true;
}

void Window::MakeContextCurrent()
{
    if(m_window == nullptr)
        return;

    glfwMakeContextCurrent(m_window);
}

void Window::ProcessEvents()
{
    if(m_window == nullptr)
        return;

    glfwPollEvents();
}

void Window::Present()
{
    if(m_window == nullptr)
        return;

    glfwSwapBuffers(m_window);

    // Check if there are any uncaught OpenGL errors.
    GLenum error;
    while((error = glGetError()) != GL_NO_ERROR)
    {
        std::stringstream stream;
        stream << std::hex << std::setfill('0') << std::setw(4) << error;

        Log() << "Found an uncaught OpenGL error in the last frame (code 0x" << stream.str() << ")!";
    }
}

void Window::Close()
{
    if(m_window == nullptr)
        return;

    glfwSetWindowShouldClose(m_window, GL_TRUE);
}

bool Window::IsOpen() const
{
    if(m_window == nullptr)
        return false;

    return glfwWindowShouldClose(m_window) == 0;
}

bool Window::IsFocused() const
{
    if(m_window == nullptr)
        return false;

    return glfwGetWindowAttrib(m_window, GLFW_FOCUSED) > 0;
}

int Window::GetWidth() const
{
    if(m_window == nullptr)
        return 0;

    int width = 0;
    glfwGetFramebufferSize(m_window, &width, nullptr);
    return width;
}

int Window::GetHeight() const
{
    if(m_window == nullptr)
        return 0;

    int height = 0;
    glfwGetFramebufferSize(m_window, nullptr, &height);
    return height;
}

GLFWwindow* Window::GetPrivateHandle()
{
    return m_window;
}
