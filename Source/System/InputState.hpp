#pragma once

#include "Precompiled.hpp"
#include "Window.hpp"

/*
    Input State

    Caches input state between frames and handles repeating keys.

    Example usage:
        System::InputState inputState;
        inputState.Initialize(...);
    
        while(true)
        {
            inputState.Update();
            window.ProcessEvents();
    
            if(window.IsKeyDown(GLFW_KEY_ESCAPE, false))
            {
                ...
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

        // Subscribes to window input events.
        bool Subscribe(Window& window);

        // Updates the input state.
        // Must be called before window events are processed.
        void Update();

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
