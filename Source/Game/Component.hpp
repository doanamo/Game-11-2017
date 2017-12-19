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

        // Called when the component needs to be finalized.
        virtual bool OnFinalize(EntityHandle self)
        {
            return true;
        }

        // Called when the component is about to be destroyed.
        virtual void OnDestroy(EntityHandle self)
        {
        }
    };
}
