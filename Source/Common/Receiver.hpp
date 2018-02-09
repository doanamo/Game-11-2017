#pragma once

#include "Precompiled.hpp"
#include "Dispatcher.hpp"
#include "Delegate.hpp"

// Forward declarations.
template<typename Type>
class DispatcherBase;

template<typename Type>
class ReceiverInvoker;

/*
    Receiver

    Invokes a delegate after receiving a signal from a dispatcher.
    Single receiver instance can be subscribed to only one dispatcher.

    See Dispatcher template class for more information.
*/

template<typename Type>
class Receiver;

template<typename ReturnType, typename... Arguments>
class Receiver<ReturnType(Arguments...)> : private Delegate<ReturnType(Arguments...)>
{
public:
    // Friend declarations.
    friend DispatcherBase<ReturnType(Arguments...)>;
    friend ReceiverInvoker<ReturnType(Arguments...)>;

public:
    Receiver() :
        m_dispatcher(nullptr),
        m_previous(nullptr),
        m_next(nullptr)
    {
    }

    virtual ~Receiver()
    {
        // Unsubscribe from the dispatcher.
        this->Unsubscribe();

        // Unbind the delegeate.
        this->Bind(nullptr);
    }

    // Subscribes to a dispatcher.
    bool Subscribe(DispatcherBase<ReturnType(Arguments...)>& dispatcher, bool replace = true)
    {
        return dispatcher.Subscribe(*this, replace);
    }

    // Unsubscribes from the current dispatcher.
    void Unsubscribe()
    {
        if(m_dispatcher != nullptr)
        {
            m_dispatcher->Unsubscribe(*this);

            Assert(m_dispatcher == nullptr, "Dispatcher did not unsubscribe this receiver properly!");
            Assert(m_previous == nullptr, "Dispatcher did not unsubscribe this receiver properly!");
            Assert(m_next == nullptr, "Dispatcher did not unsubscribe this receiver properly!");
        }
    }

    // Unbinds the receiver.
    void Bind(std::nullptr_t)
    {
        Delegate<ReturnType(Arguments...)>::Bind(nullptr);
    }
    
    // Binds a static function.
    template<ReturnType(*Function)(Arguments...)>
    void Bind()
    {
        Delegate<ReturnType(Arguments...)>::Bind<Function>();
    }

    // Binds a functor object.
    template<class InstanceType>
    void Bind(InstanceType* instance)
    {
        Delegate<ReturnType(Arguments...)>::Bind(instance);
    }

    // Binds an instance method.
    template<class InstanceType, ReturnType(InstanceType::*Function)(Arguments...)>
    void Bind(InstanceType* instance)
    {
        Delegate<ReturnType(Arguments...)>::Bind<InstanceType, Function>(instance);
    }

private:
    // Receives an event and invokes a bound function.
    ReturnType Receive(Arguments... arguments)
    {
        Assert(m_dispatcher, "Invoked a receiver without it being subscribed!");
        return this->Invoke(std::forward<Arguments>(arguments)...);
    }

private:
    // Intrusive double linked list element.
    DispatcherBase<ReturnType(Arguments...)>* m_dispatcher;
    Receiver<ReturnType(Arguments...)>*       m_previous;
    Receiver<ReturnType(Arguments...)>*       m_next;
};
