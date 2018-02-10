#pragma once

/*
    Non-copyable

    Base class that prevents an instance of a derived class from being copied.
    
    class Example : private NonCopyable
    {
        // Attempting to copy an instance of this class will generate a compile error.
        // Private inheritance of NonCopyable base class is advised over public one.
    }
*/

class NonCopyable
{
protected:
    // Default class constructor and destructor.
    NonCopyable() = default;
    ~NonCopyable() = default;

    // Delete copy constructor and operator.
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
};
