#include "Precompiled.hpp"
#include "Timer.hpp"
using namespace System;

Timer::Timer() :
    m_timerFrequency(glfwGetTimerFrequency()),
    m_currentTimeCounter(glfwGetTimerValue()),
    m_previousTimeCounter(m_currentTimeCounter),
    m_maxFrameDeltaSeconds(std::numeric_limits<float>::max())
{
    // Check if timer's frequency is valid.
    // Assertion's failure here may indicate improperly initialized platform code.
    Verify(m_timerFrequency != 0, "Failed to retrieve a valid timer frequency!");
}

Timer::~Timer()
{
}

void Timer::Reset()
{
    Assert(m_timerFrequency != 0);

    m_currentTimeCounter = glfwGetTimerValue();
    m_previousTimeCounter = m_currentTimeCounter;
}

void Timer::Tick()
{
    Assert(m_timerFrequency != 0);

    m_previousTimeCounter = m_currentTimeCounter;
    m_currentTimeCounter = glfwGetTimerValue();
}

float Timer::CalculateFrameDelta()
{
    Assert(m_timerFrequency != 0);

    // Calculate elapsed time since the last frame.
    uint64_t elapsedTimeCounter = m_currentTimeCounter - m_previousTimeCounter;

    // Calculate frame time delta in seconds.
    float frameDeltaSeconds = static_cast<float>(elapsedTimeCounter * (1.0 / m_timerFrequency));

    // Clamp delta value.
    frameDeltaSeconds = std::max(0.0f, frameDeltaSeconds);
    frameDeltaSeconds = std::min(frameDeltaSeconds, m_maxFrameDeltaSeconds);

    // Return calculated delta value.
    return frameDeltaSeconds;
}

void Timer::SetMaxFrameDelta(float value)
{
    m_maxFrameDeltaSeconds = value;
}

float Timer::GetMaxFrameDelta() const
{
    return m_maxFrameDeltaSeconds;
}
