#pragma once

#include "Precompiled.hpp"
#include "EntityHandle.hpp"

/*
    Component
    
    Base class for component types.
*/

namespace Game
{
    // Component base class.
    class Component : private NonCopyable
    {
    protected:
        Component()
        {
        }

    public:
        virtual ~Component()
        {
        }
    };
}
