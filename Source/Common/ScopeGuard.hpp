#pragma once

/*
    Scope Guard

    Executes a set function at the end of its lifetime.
    
    void ExampleScopeGuard()
    {
        // Allocate an array on the heap.
        int* array = new int[10];

        // Create a scope guard that will delete the
        // allocated memory at the end of the current scope.
        auto Cleanup = MakeScopeGuard([&]()
        {
            delete[] array;
        });
    }
    
    void ExampleScopeGuardMacro()
    {
        // Allocate an array on the heap.
        int* array = new int[10];

        // Use a scope guard macro that will delete the
        // allocated memory at the end of the current scope.
        SCOPE_GUARD(delete[] array);
    }
    
    void ExampleScopeGuardConditionalMacro()
    {
        // Create a condition for the scope guard.
        bool cleanup = true;

        // Allocate an array on the heap.
        int* array = new int[10];

        // Use a conditional scope guard macro that will delete the
        // allocated memory at the end of the current scope.
        SCOPE_GUARD_IF(cleanup, delete[] array);
    }
    
    void ExampleScopeGuardBracedMacro()
    {
        // Allocate an array on the heap.
        int* array = new int[10];

        // Use a braced scope guard macro that will delete the
        // allocated memory at the end of the current scope.
        SCOPE_GUARD_BEGIN();
        {
            delete[] array;
            array = nullptr;
        }
        SCOPE_GUARD_END();
    }
    
    void ExampleScopeGuardConditionalBracedMacro()
    {
        // Create a condition for the scope guard.
        bool cleanup = true;

        // Allocate an array on the heap.
        int* array = new int[10];

        // Use a conditional braced scope guard macro that will delete
        // the allocated memory at the end of the current scope.
        SCOPE_GUARD_BEGIN(cleanup);
        {
            delete[] array;
            array = nullptr;
        }
        SCOPE_GUARD_END();
*/

// Scope guard template class.
template<typename Type>
class ScopeGuard : private NonCopyable
{
public:
    explicit ScopeGuard(Type function) :
        m_function(function)
    {
    }

    ScopeGuard(ScopeGuard<Type>&& other)
    {
        m_function = std::move(other.m_function);
    }

    ~ScopeGuard()
    {
        m_function();
    }

public:
    // Helper boolean wrapper.
    class Condition
    {
    public:
        Condition() :
            m_value(true)
        {
        }

        Condition(bool value) :
            m_value(value)
        {
        }

        explicit operator bool()
        {
            return m_value;
        }

    private:
        bool m_value;
    };

private:
    // Function that will be called.
    Type m_function;
};

// Creates a scope guard.
template<typename Type>
ScopeGuard<Type> MakeScopeGuard(Type function)
{
    return ScopeGuard<Type>(function);
}

// Utility macros.
#define SCOPE_GUARD_BEGIN(...) auto SCOPE_GUARD_NAME(__LINE__) = MakeScopeGuard([&]() { if(ScopeGuard<void>::Condition(__VA_ARGS__)) { 
#define SCOPE_GUARD_END() } });

#define SCOPE_GUARD(code) auto SCOPE_GUARD_NAME(__LINE__) = SCOPE_GUARD_MAKE(code)
#define SCOPE_GUARD_MAKE(code) MakeScopeGuard([&]() { code; })

#define SCOPE_GUARD_IF(condition, code) auto SCOPE_GUARD_NAME(__LINE__) = SCOPE_GUARD_IF_MAKE(condition, code)
#define SCOPE_GUARD_IF_MAKE(condition, code) MakeScopeGuard([&]() { if(condition) { code; } })

#define SCOPE_GUARD_NAME(line) SCOPE_GUARD_STRING(line)
#define SCOPE_GUARD_STRING(line) scopeguardLine ## line
