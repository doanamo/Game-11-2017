#include "Precompiled.hpp"

int main(int argc, char* argv[])
{
    Build::Initialize();
    Debug::Initialize();
    Logger::Initialize();

    // Initialize GLFW library.
    glewInit();

    if(!glfwInit())
    {
        Log() << "Failed to initialize GLFW library!";
        return -1;
    }

    SCOPE_GUARD(glfwTerminate());

    // Create a window.
    GLFWwindow* window = glfwCreateWindow(1024, 576, "Game", nullptr, nullptr);

    if(!window)
    {
        Log() << "Failed to create a window!";
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Initialize GLEW library.
    GLenum error = glewInit();

    if(error != GLEW_OK)
    {
        Log() << "GLEW Error: " << glewGetErrorString(error);
        Log() << "Couldn't initialize GLEW library.";
        return -1;
    }

    // Main application loop.
    while(!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}
