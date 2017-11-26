#include "Precompiled.hpp"
#include "System/Context.hpp"
#include "System/Config.hpp"
#include "System/Timer.hpp"
#include "System/Window.hpp"

#include "Graphics/Buffer.hpp"
#include "Graphics/VertexInput.hpp"
#include "Graphics/Shader.hpp"

namespace
{
    // Log error messages.
    #define LogFatalError() "Fatal error has been encountered! "
}

int main(int argc, char* argv[])
{
    // Initialize core systems.
    Build::Initialize();
    Debug::Initialize();
    Logger::Initialize();

    // Initialize platform context.
    System::Context context;

    if(!context.Initialize())
    {
        Log() << LogFatalError() << "Could not initialize the context.";
        return -1;
    }

    // Instantiate a config instance.
    System::Config config;
    config.LoadFromFile("Game.cfg");

    // Instantiate a window instance.
    System::WindowInfo windowInfo;
    windowInfo.name = "Game";
    windowInfo.width = config.GetParameter<int>("Window.Width", 1024);
    windowInfo.height = config.GetParameter<int>("Window.Height", 576);
    windowInfo.vsync = config.GetParameter<bool>("Window.Vsync", true);

    System::Window window;
    if(!window.Open(windowInfo))
    {
        Log() << LogFatalError() << "Could not open a window.";
        return -1;
    }

    // Instantiate a timer instance.
    System::Timer timer;
    if(!timer.IsFrequencyValid())
    {
        Log() << LogFatalError() << "Could not create a valid a timer.";
        return -1;
    }

    timer.SetMaxFrameDelta(1.0f);

    // Create a vertex buffer.
    struct Vertex
    {
        glm::vec3 position;
    };

    const Vertex vertices[] =
    {
        { glm::vec3(-1.0f, -1.0f, 0.0f) },
        { glm::vec3( 1.0f, -1.0f, 0.0f) },
        { glm::vec3( 0.0f,  1.0f, 0.0f) },
    };

    Graphics::BufferInfo bufferInfo;
    bufferInfo.elementSize = sizeof(Vertex);
    bufferInfo.elementCount = Utility::ArraySize(vertices);
    bufferInfo.data = &vertices[0];

    Graphics::VertexBuffer vertexBuffer;
    if(!vertexBuffer.Create(bufferInfo))
    {
        return -1;
    }

    // Create a vertex input.
    const Graphics::VertexAttribute vertexAttributes[] =
    {
        { &vertexBuffer, Graphics::VertexAttributeTypes::Float3 }, // Position
    };

    Graphics::VertexInputInfo vertexInputInfo;
    vertexInputInfo.attributeCount = Utility::ArraySize(vertexAttributes);
    vertexInputInfo.attributes = &vertexAttributes[0];

    Graphics::VertexInput vertexInput;
    if(!vertexInput.Create(vertexInputInfo))
    {
        return -1;
    }

    // Create a shader.
    Graphics::Shader shader;
    if(!shader.Load("Data/Shaders/Basic.shader"))
    {
        return -1;
    }

    // Main loop.
    while(window.IsOpen())
    {
        // Advance logger's frame of reference.
        Logger::AdvanceFrameReference();

        // Calculate frame delta time.
        float frameDelta = timer.CalculateFrameDelta();

        // Process window evenets.
        window.ProcessEvents();

        // Clear the framebuffer.
        glClearColor(0.0f, 0.35f, 0.35f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw a triangle.
        glUseProgram(shader.GetHandle());
        glUniformMatrix4fv(shader.GetUniform("vertexTransform"), 1, GL_FALSE, glm::value_ptr(glm::mat4()));

        glBindVertexArray(vertexInput.GetHandle());

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(0);

        glUseProgram(0);

        // Present to the window.
        window.Present();

        // Tick the timer.
        timer.Tick();
    }

    return 0;
}
