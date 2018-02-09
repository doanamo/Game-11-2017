#include "Precompiled.hpp"
#include "InputState.hpp"
#include "Window.hpp"
using namespace System;

InputState::InputState()
{
    // Setup initial input state.
    this->Reset();

    // Bind event receivers.
    m_keyboardKey.Bind<InputState, &InputState::OnKeyboardKey>(this);
    m_windowFocus.Bind<InputState, &InputState::OnWindowFocus>(this);
}

InputState::~InputState()
{
}

bool InputState::Subscribe(Window& window)
{
    Log() << "Subscribing input state..." << LogIndent();

    // Subscribe to window's event receivers.
    bool subscribedSuccessfully = true;

    subscribedSuccessfully &= m_keyboardKey.Subscribe(window.events.keyboardKey);
    subscribedSuccessfully &= m_windowFocus.Subscribe(window.events.focus);

    if(!subscribedSuccessfully)
    {
        LogError() << "Could not subscribe to window's event receivers!";
        return false;
    }

    // Success!
    Log() << "Success!";

    return true;
}

void InputState::OnKeyboardKey(const Window::Events::KeyboardKey& event)
{
    Verify(0 <= event.key && event.key < KeyboardKeyCount, "Received an event with an invalid key!");

    // Handle keyboard input events.
    if(event.action == GLFW_PRESS)
    {
        m_keyboardState[event.key] = KeyboardKeyStates::Pressed;
    }
    else if(event.action == GLFW_RELEASE)
    {
        m_keyboardState[event.key] = KeyboardKeyStates::Released;
    }
}

void InputState::OnWindowFocus(const Window::Events::Focus& event)
{
    // Reset key states on focus loss.
    // This will help avoid stuck input states after alt + tabing between application.
    if(!event.focused)
    {
        this->Reset();
    }
}

void InputState::Prepare()
{
    // Update repeating key states.
    for(int i = 0; i < KeyboardKeyCount; ++i)
    {
        auto& state = m_keyboardState[i];

        if(state == KeyboardKeyStates::Pressed)
        {
            state = KeyboardKeyStates::PressedRepeat;
        }
        else if(state == KeyboardKeyStates::Released)
        {
            state = KeyboardKeyStates::ReleasedRepeat;
        }
    }
}

void InputState::Reset()
{
    // Set all keys to their untouched state.
    for(int i = 0; i < KeyboardKeyCount; ++i)
    {
        m_keyboardState[i] = KeyboardKeyStates::ReleasedRepeat;
    }
}

bool InputState::IsKeyboardKeyDown(int key, bool repeat)
{
    Verify(0 <= key && key < KeyboardKeyCount, "Attempting to index an invalid key!");

    // Check if the key was just pressed.
    if(m_keyboardState[key] == KeyboardKeyStates::Pressed)
        return true;

    // Check if the key is in repeating pressed state.
    if(repeat && m_keyboardState[key] == KeyboardKeyStates::PressedRepeat)
        return true;

    return false;
}

bool InputState::IsKeyboardKeyUp(int key, bool repeat)
{
    Verify(0 <= key && key < KeyboardKeyCount, "Attempting to index an invalid key!");

    // Check if the key was just released.
    if(m_keyboardState[key] == KeyboardKeyStates::Released)
        return true;

    // Check if the key is in repeating released state.
    if(repeat && m_keyboardState[key] == KeyboardKeyStates::ReleasedRepeat)
        return true;

    return false;
}
