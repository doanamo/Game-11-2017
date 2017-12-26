#pragma once

#include "Precompiled.hpp"
#include "EntityHandle.hpp"
#include "Component.hpp"

/*
    Component Pool
    
    Manages a single type of a component.
    See ComponentSystem for more context.
*/

namespace Game
{
    // Component pool interface class.
    class ComponentPoolInterface
    {
    protected:
        ComponentPoolInterface()
        {
        }

    public:
        virtual ~ComponentPoolInterface()
        {
        }

        virtual bool Destroy(EntityHandle handle) = 0;
    };

    // Component pool class.
    template<typename Type>
    class ComponentPool : public ComponentPoolInterface
    {
    public:
        // Check template type.
        static_assert(std::is_base_of<Component, Type>::value, "Not a component type.");

        // Type declarations.
        typedef std::unordered_map<EntityHandle, Type> ComponentList;
        typedef typename ComponentList::iterator       ComponentIterator;

    public:
        ComponentPool();
        ~ComponentPool();

        // Creates a component.
        // Returns nullptr if component could not be created.
        Type* Create(EntityHandle handle);

        // Lookups a component.
        // Returns nullptr if component could not be found.
        Type* Lookup(EntityHandle handle);

        // Destroys a component.
        // Returns true if component was found and destroyed.
        bool Destroy(EntityHandle handle) override;

        // Gets the begin iterator.
        ComponentIterator Begin();

        // Gets the end iterator.
        ComponentIterator End();

    private:
        // List of components.
        ComponentList m_components;
    };

    // Template definitions.
    template<typename Type>
    ComponentPool<Type>::ComponentPool()
    {
    }

    template<typename Type>
    ComponentPool<Type>::~ComponentPool()
    {
    }

    template<typename Type>
    Type* ComponentPool<Type>::Create(EntityHandle handle)
    {
        // Create a new component for this entity handle.
        auto result = m_components.emplace(std::piecewise_construct, std::forward_as_tuple(handle), std::forward_as_tuple());

        // There may already be a component associated with this handle.
        if(result.second == false)
            return nullptr;

        // Return a pointer to a newly created component.
        return &result.first->second;
    }

    template<typename Type>
    Type* ComponentPool<Type>::Lookup(EntityHandle handle)
    {
        // Find the component.
        auto it = m_components.find(handle);
        if(it == m_components.end())
            return nullptr;

        // Return a pointer to the component.
        return &it->second;
    }

    template<typename Type>
    bool ComponentPool<Type>::Destroy(EntityHandle handle)
    {
        // Find the component.
        auto it = m_components.find(handle);
        if(it == m_components.end())
            return false;

        // Destroy the associated component.
        m_components.erase(it);

        return true;
    }

    template<typename Type>
    typename ComponentPool<Type>::ComponentIterator ComponentPool<Type>::Begin()
    {
        return m_components.begin();
    }

    template<typename Type>
    typename ComponentPool<Type>::ComponentIterator ComponentPool<Type>::End()
    {
        return m_components.end();
    }
}
