#include "Precompiled.hpp"
#include "Window.hpp"
#include "Config.hpp"
using namespace System;

namespace
{
    // Error log messages.
    #define LogOpenError() "Failed to open a window! "
}

WindowInfo::WindowInfo() :
    title("Window"),
    width(1024),
    height(576),
    vsync(true),
    minWidth(GLFW_DONT_CARE),
    minHeight(GLFW_DONT_CARE),
    maxWidth(GLFW_DONT_CARE),
    maxHeight(GLFW_DONT_CARE)
{
}

Window::Window() :
    m_window(nullptr),
    m_title(""),
    m_sizeChanged(false)
{
}

Window::~Window()
{
    // We have to destroy an allocated handle.
    this->DestroyWindow();
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

void Window::MoveCallback(GLFWwindow* window, int x, int y)
{
    Assert(window != nullptr, "Window handle is nullptr!");

    // Get the window instance.
    auto instance = reinterpret_cast<System::Window*>(glfwGetWindowUserPointer(window));
    Assert(instance != nullptr, "Window instance is nullptr!");

    // Send an event.
    Window::Events::Move eventData;
    eventData.x = x;
    eventData.y = y;

    instance->events.move(eventData);
}

void Window::ResizeCallback(GLFWwindow* window, int width, int height)
{
    Assert(window != nullptr, "Window reference is nullptr!");

    // Get the window instance.
    auto instance = reinterpret_cast<System::Window*>(glfwGetWindowUserPointer(window));
    Assert(instance != nullptr, "Window instance is nullptr!");

    // Remember that window size has changed.
    instance->m_sizeChanged = true;

    // Send an event.
    Window::Events::Resize eventData;
    eventData.width = width;
    eventData.height = height;

    instance->events.resize(eventData);
}

void Window::FocusCallback(GLFWwindow* window, int focused)
{
    Assert(window != nullptr, "Window reference is nullptr!");

    // Get the window instance.
    auto instance = reinterpret_cast<System::Window*>(glfwGetWindowUserPointer(window));
    Assert(instance != nullptr, "Window instance is nullptr!");

    // Send an event.
    Window::Events::Focus eventData;
    eventData.focused = focused > 0;

    instance->events.focus(eventData);
}

void Window::CloseCallback(GLFWwindow* window)
{
    Assert(window != nullptr, "Window reference is nullptr!");

    // Get the window instance.
    auto instance = reinterpret_cast<System::Window*>(glfwGetWindowUserPointer(window));
    Assert(instance != nullptr, "Window instance is nullptr!");

    // Send an event.
    Window::Events::Close eventData;

    instance->events.close(eventData);
}

void Window::KeyboardKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Assert(window != nullptr, "Window reference is nullptr!");

    // Get the window instance.
    auto instance = reinterpret_cast<System::Window*>(glfwGetWindowUserPointer(window));
    Assert(instance != nullptr, "Window instance is nullptr!");

    // Send an event.
    Window::Events::KeyboardKey eventData;
    eventData.key = key;
    eventData.scancode = scancode;
    eventData.action = action;
    eventData.mods = mods;

    instance->events.keyboardKey(eventData);
}

void Window::TextInputCallback(GLFWwindow* window, unsigned int character)
{
    Assert(window != nullptr, "Window reference is nullptr!");

    // Get the window instance.
    auto instance = reinterpret_cast<System::Window*>(glfwGetWindowUserPointer(window));
    Assert(instance != nullptr, "Window instance is nullptr!");

    // Send an event.
    Window::Events::TextInput eventData;
    eventData.character = character;

    instance->events.textInput(eventData);
}

void Window::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    Assert(window != nullptr, "Window reference is nullptr!");

    // Get the window instance.
    auto instance = reinterpret_cast<System::Window*>(glfwGetWindowUserPointer(window));
    Assert(instance != nullptr, "Window instance is nullptr!");

    // Send an event.
    Window::Events::MouseButton eventData;
    eventData.button = button;
    eventData.action = action;
    eventData.mods = mods;

    instance->events.mouseButton(eventData);
}

void Window::MouseScrollCallback(GLFWwindow* window, double offsetx, double offsety)
{
    Assert(window != nullptr, "Window reference is nullptr!");

    // Get the window instance.
    auto instance = reinterpret_cast<System::Window*>(glfwGetWindowUserPointer(window));
    Assert(instance != nullptr, "Window instance is nullptr!");

    // Send an event.
    Window::Events::MouseScroll eventData;
    eventData.offset = offsety;

    instance->events.mouseScroll(eventData);
}

void Window::CursorPositionCallback(GLFWwindow* window, double x, double y)
{
    Assert(window != nullptr, "Window reference is nullptr!");

    // Get the window instance.
    auto instance = reinterpret_cast<System::Window*>(glfwGetWindowUserPointer(window));
    Assert(instance != nullptr, "Window instance is nullptr!");

    // Send an event.
    Window::Events::CursorPosition eventData;
    eventData.x = x;
    eventData.y = y;

    instance->events.cursorPosition(eventData);
}

void Window::CursorEnterCallback(GLFWwindow* window, int entered)
{
    Assert(window != nullptr, "Window reference is nullptr!");

    // Get the window's instance.
    auto instance = reinterpret_cast<System::Window*>(glfwGetWindowUserPointer(window));
    Assert(instance != nullptr, "Window instance is nullptr!");

    // Send an event.
    Window::Events::CursorEnter eventData;
    eventData.entered = entered != 0;

    instance->events.cursorEnter(eventData);
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
    m_window = glfwCreateWindow(info.width, info.height, info.title.c_str(), nullptr, nullptr);

    if(m_window == nullptr)
    {
        Log() << LogOpenError() << "Could not create the window.";
        return false;
    }

    SCOPE_GUARD_IF(!initialized, this->DestroyWindow());

    // Set window size limits.
    glfwSetWindowSizeLimits(m_window, info.minWidth, info.minHeight, info.maxWidth, info.maxHeight);

    // Set window user data.
    glfwSetWindowUserPointer(m_window, this);

    // Add event callbacks.
    glfwSetWindowPosCallback(m_window, Window::MoveCallback);
    glfwSetFramebufferSizeCallback(m_window, Window::ResizeCallback);
    glfwSetWindowFocusCallback(m_window, Window::FocusCallback);
    glfwSetWindowCloseCallback(m_window, Window::CloseCallback);
    glfwSetKeyCallback(m_window, Window::KeyboardKeyCallback);
    glfwSetCharCallback(m_window, Window::TextInputCallback);
    glfwSetMouseButtonCallback(m_window, Window::MouseButtonCallback);
    glfwSetScrollCallback(m_window, Window::MouseScrollCallback);
    glfwSetCursorPosCallback(m_window, Window::CursorPositionCallback);
    glfwSetCursorEnterCallback(m_window, Window::CursorEnterCallback);

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

    // Store window's title as it cannot be retrived back via GLFW.
    m_title = info.title;

    // Success!
    return initialized = true;
}

void Window::MakeContextCurrent()
{
    if(m_window == nullptr)
        return;

    // Mark associated OpenGL context as current.
    glfwMakeContextCurrent(m_window);
}

void Window::ProcessEvents()
{
    if(m_window == nullptr)
        return;

    // Poll and process events using callbacks.
    glfwPollEvents();

    // Log window size change.
    if(m_sizeChanged)
    {
        int windowWidth, windowHeight;
        glfwGetFramebufferSize(m_window, &windowWidth, &windowHeight);

        Log() << "Window has been resized to " << windowWidth << "x" << windowHeight << ".";

        m_sizeChanged = false;
    }
}

void Window::Present()
{
    if(m_window == nullptr)
        return;

    // Swap framebuffers.
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

    // Force the window to close.
    glfwSetWindowShouldClose(m_window, GL_TRUE);
}

void Window::SetTitle(std::string title)
{
    if(m_window == nullptr)
        return;

    // Set the window's new title.
    glfwSetWindowTitle(m_window, title.c_str());

    // Cache the window's title as it cannot be retrieved back via GLFW.
    m_title = title;
}

bool Window::IsOpen() const
{
    if(m_window == nullptr)
        return false;

    // Window is considered open as long as there was no request made to close it.
    return glfwWindowShouldClose(m_window) == 0;
}

bool Window::IsFocused() const
{
    if(m_window == nullptr)
        return false;

    // Check if the window is currently in the foreground.
    return glfwGetWindowAttrib(m_window, GLFW_FOCUSED) > 0;
}

std::string Window::GetTitle() const
{
    // Return the cached window's title.
    return m_title;
}

int Window::GetWidth() const
{
    if(m_window == nullptr)
        return 0;

    // Return the current framebuffer's width.
    int width = 0;
    glfwGetFramebufferSize(m_window, &width, nullptr);
    return width;
}

int Window::GetHeight() const
{
    if(m_window == nullptr)
        return 0;

    // Return the current framebuffer's height.
    int height = 0;
    glfwGetFramebufferSize(m_window, nullptr, &height);
    return height;
}

GLFWwindow* Window::GetPrivateHandle()
{
    // Return the private window handle.
    return m_window;
}
