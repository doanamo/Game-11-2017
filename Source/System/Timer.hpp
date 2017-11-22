#pragma once

#include "Precompiled.hpp"

/*
    Timer

    Keeps track of time and provides utilities such as automatic
    calculation of delta time between ticks or frame rate.

    Exmaple usage:
        System::Timer timer;

        while(true)
        {
            float dt = timer.CalculateFrameDelta();

            // ...

            timer.Tick();
        }
*/

namespace System
{
    // Timer class.
    class Timer
    {
    public:
        Timer();
        ~Timer();

        // Restores class instance to its original state.
        void Cleanup();

        // Resets the timer.
        void Reset();

        // Ticks the timer.
        void Tick();

        // Calculates frame delta time in seconds between last two ticks.
        float CalculateFrameDelta();

        // Sets maximum frame delta in seconds that can be returned.
        void SetMaxFrameDelta(float value);

        // Gets maximum frame delta in seconds that can be returned.
        float GetMaxFrameDelta() const;

        // Checks if timer's frequency is valid.
        bool IsFrequencyValid() const;

    private:
        uint64_t m_timerFrequency;
        uint64_t m_currentTimeCounter;
        uint64_t m_previousTimeCounter;

        float m_maxFrameDeltaSeconds;
    };
}
