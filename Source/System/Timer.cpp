#include "Precompiled.hpp"
#include "Timer.hpp"
using namespace System;

Timer::Timer() :
    m_secondsPerCounter(0),
    m_currentTimeCounter(0),
    m_previousTimeCounter(0),
    m_maxFrameDeltaSeconds(std::numeric_limits<float>::max()),
    m_initialized(false)
{
}

Timer::~Timer()
{
    this->Cleanup();
}

void Timer::Cleanup()
{
    if(!m_initialized)
        return;

    m_secondsPerCounter = 0;

    m_currentTimeCounter = 0;
    m_previousTimeCounter = 0;

    m_maxFrameDeltaSeconds = std::numeric_limits<float>::max();

    m_initialized = false;
}

bool Timer::Initialize()
{
    Cleanup();

    // Setup a cleanup guard.
    SCOPE_GUARD
    (
        if(!m_initialized)
        {
            m_initialized = true;
            this->Cleanup();
        }
    );

    // Query the timer's frequency.
    uint64_t timerFrequency = glfwGetTimerFrequency();

    if(timerFrequency == 0)
    {
        Log() << "Failed to initialize a timer! Could not get a valid timer frequency.";
        return false;
    }

    m_secondsPerCounter = 1.0 / timerFrequency;
   
    // Setup initial values on first tick.
    m_currentTimeCounter = glfwGetTimerValue();
    m_previousTimeCounter = m_currentTimeCounter;

    // Success!
    return m_initialized = true;
}

void Timer::Reset()
{
    if(!m_initialized)
        return;

    m_currentTimeCounter = glfwGetTimerValue();
    m_previousTimeCounter = m_currentTimeCounter;
}

void Timer::Tick()
{
    if(!m_initialized)
        return;

    m_previousTimeCounter = m_currentTimeCounter;
    m_currentTimeCounter = glfwGetTimerValue();
}

float Timer::CalculateFrameDelta()
{
    if(!m_initialized)
        return 0.0f;

    // Calculate elapsed time since the last frame.
    uint64_t elapsedTimeCounter = m_currentTimeCounter - m_previousTimeCounter;

    // Calculate frame time delta in seconds.
    float frameDeltaSeconds = static_cast<float>(elapsedTimeCounter * m_secondsPerCounter);

    // Clamp delta value.
    frameDeltaSeconds = std::max(0.0f, frameDeltaSeconds);
    frameDeltaSeconds = std::min(frameDeltaSeconds, m_maxFrameDeltaSeconds);

    // Return calculated value.
    return frameDeltaSeconds;
}

void Timer::SetMaxFrameDelta(float value)
{
    if(!m_initialized)
        return;

    m_maxFrameDeltaSeconds = value;
}

float Timer::GetMaxFrameDelta() const
{
    return m_maxFrameDeltaSeconds;
}
