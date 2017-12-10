#include "Precompiled.hpp"
#include "System/Context.hpp"
#include "System/Config.hpp"
#include "System/Timer.hpp"
#include "System/Window.hpp"
#include "System/ResourceManager.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/VertexInput.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/Sampler.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/ScreenSpace.hpp"

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

    // Create a config.
    System::Config config;
    config.LoadFromFile("Game.cfg");

    // Create a window.
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

    // Create a timer.
    System::Timer timer;
    if(!timer.IsFrequencyValid())
    {
        Log() << LogFatalError() << "Could not create a valid a timer.";
        return -1;
    }

    timer.SetMaxFrameDelta(1.0f);

    // Create a resource manager.
    System::ResourceManager resourceManager;

    // Create a sampler.
    Graphics::SamplerInfo samplerInfo;

    Graphics::Sampler sampler;
    if(!sampler.Create(samplerInfo))
    {
        return -1;
    }

    // Create a vertex buffer.
    struct Vertex
    {
        glm::vec3 position;
        glm::vec2 texture;
    };

    const Vertex vertices[] =
    {
        { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0, 0.0f) },
        { glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0, 1.0f) },
        { glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(1.0, 1.0f) },

        { glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(1.0, 1.0f) },
        { glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0, 0.0f) },
        { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0, 0.0f) },
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
        { &vertexBuffer, Graphics::VertexAttributeTypes::Float2 }, // Texture
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
    auto shader = resourceManager.Load<Graphics::Shader>("Data/Shaders/Basic.shader");

    // Create a texture.
    auto texture = resourceManager.Load<Graphics::Texture>("Data/Textures/ColorCheckerboard.png");

    // Create a screen space coordinates.
    Graphics::ScreenSpace screenSpace;
    screenSpace.SetSourceSize(1.0f, 1.0f);

    // Main loop.
    while(window.IsOpen())
    {
        // Advance logger's frame of reference.
        Logger::AdvanceFrameReference();

        // Calculate frame delta time.
        float frameDelta = timer.CalculateFrameDelta();

        // Process window evenets.
        window.ProcessEvents();

        // Setup viewport size.;
        glViewport(0, 0, window.GetWidth(), window.GetHeight());

        // Update screen space coordinates.
        screenSpace.SetTargetSize(window.GetWidth(), window.GetHeight());

        // Clear the framebuffer.
        glClearColor(0.0f, 0.35f, 0.35f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw a triangle.
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture->GetHandle());

        glUseProgram(shader->GetHandle());
        glUniformMatrix4fv(shader->GetUniform("vertexTransform"), 1, GL_FALSE, glm::value_ptr(screenSpace.GetTransform()));
        glUniform1i(shader->GetUniform("textureDiffuse"), 0);

        glBindVertexArray(vertexInput.GetHandle());

        glDrawArrays(GL_TRIANGLES, 0, vertexBuffer.GetElementCount());

        glBindVertexArray(0);

        glUseProgram(0);

        glBindTexture(GL_TEXTURE_2D, 0);

        // Present to the window.
        window.Present();

        // Tick the timer.
        timer.Tick();
    }

    return 0;
}
