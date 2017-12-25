#include "Precompiled.hpp"
#include "InputState.hpp"
#include "Window.hpp"
using namespace System;

InputState::InputState()
{
    // Set initial input states.
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
    // Subscribe event receivers.
    if(!m_keyboardKey.Subscribe(window.events.keyboardKey) || !m_windowFocus.Subscribe(window.events.focus))
    {
        Log() << "Failed to subscribe an input state! Could not subscribe to window event receivers.";
        return false;
    }

    return true;
}

void InputState::OnKeyboardKey(const Window::Events::KeyboardKey& event)
{
    Assert(0 <= event.key && event.key < KeyboardKeyCount);

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
    if(!event.focused)
    {
        this->Reset();
    }
}

void InputState::Update()
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
    // Set all keys as released.
    for(int i = 0; i < KeyboardKeyCount; ++i)
    {
        m_keyboardState[i] = KeyboardKeyStates::ReleasedRepeat;
    }
}

bool InputState::IsKeyDown(int key, bool repeat)
{
    if(key < 0 || key >= KeyboardKeyCount)
        return false;

    if(m_keyboardState[key] == KeyboardKeyStates::Pressed)
        return true;

    if(repeat && m_keyboardState[key] == KeyboardKeyStates::PressedRepeat)
        return true;

    return false;
}

bool InputState::IsKeyUp(int key, bool repeat)
{
    if(key < 0 || key >= KeyboardKeyCount)
        return false;

    if(m_keyboardState[key] == KeyboardKeyStates::Released)
        return true;

    if(repeat && m_keyboardState[key] == KeyboardKeyStates::ReleasedRepeat)
        return true;

    return false;
}
