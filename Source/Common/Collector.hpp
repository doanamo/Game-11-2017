#pragma once

#include "Precompiled.hpp"

/*
    Collector Templates
    
    Utility classes for storing return values of multiple invocations.
    Returns boolean on a call that indicates the result of invocations.

    void ExampleCollector()
    {
        // Definition of a dispatcher that will keep invoking receivers
        // as long as they keep returning true as the result.
        Dispatcher<bool(void), CollectWhileTrue<bool>> dispatcher;

        // Returns true if all receivers returned true.
        // Returns false when the first receiver returns false.
        bool result = dispatcher.Dispatch();
    }
*/

// Default collector.
template<typename ReturnType>
class CollectLast;

template<typename Type>
class CollectDefault : public CollectLast<Type>
{
};

// Default collector specialized for dealing with void return type.
template<>
class CollectDefault<void>
{
public:
    bool operator()(void)
    {
        return true;
    }

    void GetResult() const
    {
    }
};

// Collector that returns the result of the last receiver invocation.
template<typename ReturnType>
class CollectLast
{
public:
    CollectLast() :
        m_result()
    {
    }

    bool operator()(ReturnType result)
    {
        m_result = result;
        return true;
    }

    ReturnType GetResult() const
    {
        return m_result;
    }

private:
    ReturnType m_result;
};

// Collector that continues dispatcher propagation while receiver invocations return true.
template<typename ReturnType = bool>
class CollectWhileTrue
{
public:
    CollectWhileTrue() :
        m_result()
    {
    }

    bool operator()(ReturnType result)
    {
        return m_result = result ? true : false;
    }

    ReturnType GetResult() const
    {
        return m_result;
    }

private:
    ReturnType m_result;
};

// Collector that continues dispatcher propagation while receiver invocations return false.
template<typename ReturnType = bool>
class CollectWhileFalse
{
public:
    public:
    CollectWhileFalse() :
        m_result()
    {
    }

    bool operator()(ReturnType result)
    {
        return m_result = result ? false : true;
    }

    ReturnType GetResult() const
    {
        return m_result;
    }

private:
    ReturnType m_result;
};
