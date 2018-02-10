#pragma once

#include "Precompiled.hpp"
#include "Window.hpp"

/*
    System Input State

    Caches input state between frames and handles repeating keys.
    Allows various input states to be easily polled.

    void ExampleSystemInputState(System::Window& window)
    {
        // Create an input state instance.
        System::InputState inputState;

        // Subscrube the input state to window's input events.
        inputState.Subscribe(window);
    
        // Run the main window loop.
        while(window.IsOpen())
        {
            // Prepare the state for incoming events.
            inputState.Prepare();

            // Process events that will be dispatched to the input state.
            window.ProcessEvents();
    
            // Check if the escape key was pressed once.
            if(window.IsKeyDown(GLFW_KEY_ESCAPE, false))
            {
                Log() << "Escape key has been pressed!";
            }
        }
    }
*/

namespace System
{
    // Forward declarations.
    class Window;

    // Input state class.
    class InputState
    {
    public:
        // Constant variables.
        static const int KeyboardKeyCount = GLFW_KEY_LAST + 1;

        // Keyboard key states.
        struct KeyboardKeyStates
        {
            enum Type
            {
                Pressed,
                PressedRepeat,
                Released,
                ReleasedRepeat,
            };
        };

    public:
        InputState();
        ~InputState();

        // Subscribes to window's input events.
        bool Subscribe(Window& window);

        // Prepares the input state for incoming input events.
        // Must be called before window events are processed.
        void Prepare();

        // Resets the input state.
        void Reset();

        // Checks if a keyboard key is down.
        bool IsKeyboardKeyDown(int key, bool repeat = true);

        // Checks if a keyboard key is up.
        bool IsKeyboardKeyUp(int key, bool repeat = true);

    private:
        // Called when a keyboard key is pressed.
        void OnKeyboardKey(const Window::Events::KeyboardKey& event);

        // Called when the window changes focus.
        void OnWindowFocus(const Window::Events::Focus& event);

    private:
        // Event receivers.
        Receiver<void(const Window::Events::KeyboardKey&)> m_keyboardKey;
        Receiver<void(const Window::Events::Focus&)>       m_windowFocus;
        
        // States of keyboard keys.
        KeyboardKeyStates::Type m_keyboardState[KeyboardKeyCount];
    };
}
