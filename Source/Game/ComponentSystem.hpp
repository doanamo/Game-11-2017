#pragma once

#include "Precompiled.hpp"
#include "EntityHandle.hpp"
#include "Component.hpp"
#include "ComponentPool.hpp"

/*
    Component System

    Manages component instances and ties them to entities.

    Adding components to entities.
        EntityHandle entity = entitySystem.CreateEntity();
        auto transform = componentSystem.Create<Game::Components::Transform>(entity);
        auto Render = componentSystem.Create<Game::Components::Render>(entity);
        entitySystem.ProcessCommands();

    Declaring a component type:
        class Class : public Game::Component
        {
            ...
        };

    Iterate over all components of a type:
        auto componentsBegin = m_componentSystem->Begin<Components::Class>();
        auto componentsEnd = m_componentSystem->End<Components::Class>();
    
        for(auto it = componentsBegin; it != componentsEnd; ++it)
        {
            const EntityHandle& entity = it->first;
            Components::Class& component = it->second;
        
            ...
        }
*/

namespace Game
{
    // Forward declarations.
    class EntitySystem;

    // Component system class.
    class ComponentSystem
    {
    public:
        // Type declarations.
        typedef std::unique_ptr<ComponentPoolInterface>               ComponentPoolPtr;
        typedef std::unordered_map<std::type_index, ComponentPoolPtr> ComponentPoolList;
        typedef ComponentPoolList::value_type                         ComponentPoolPair;

    public:
        // Following event receivers must be subscribed to the entity system.
        struct EventReceivers
        {
            Receiver<void(EntityHandle)> entityDestroy;
        } eventReceivers;

    public:
        ComponentSystem();
        ~ComponentSystem();

        // Subscribes to events dispatched by the entity system.
        bool Subscribe(EntitySystem& entitySystem);

        // Creates a component.
        template<typename Type>
        Type* Create(EntityHandle handle);

        // Lookups a component.
        template<typename Type>
        Type* Lookup(EntityHandle handle);

        // Destroys a component.
        template<typename Type>
        bool Destroy(EntityHandle handle);

        // Gets the begin iterator.
        template<typename Type>
        typename ComponentPool<Type>::ComponentIterator Begin();

        // Gets the end iterator.
        template<typename Type>
        typename ComponentPool<Type>::ComponentIterator End();

        // Gets a component pool.
        template<typename Type>
        ComponentPool<Type>* GetPool();

    private:
        // Creates a component type.
        template<typename Type>
        ComponentPool<Type>* CreatePool();

    private:
        // Called when an entity is about to be destroyed.
        void OnEntityDestroy(EntityHandle handle);

    private:
        // Component pools.
        ComponentPoolList m_pools;
    };

    // Template definitions.
    template<typename Type>
    Type* ComponentSystem::Create(EntityHandle handle)
    {
        // Validate component type.
        static_assert(std::is_base_of<Component, Type>::value, "Not a component type.");

        // Get the component pool.
        ComponentPool<Type>* pool = this->GetPool<Type>();
        Assert(pool != nullptr, "Retrieved a null component pull!");

        // Create and return a new component.
        return pool->Create(handle);
    }

    template<typename Type>
    Type* ComponentSystem::Lookup(EntityHandle handle)
    {
        // Validate component type.
        static_assert(std::is_base_of<Component, Type>::value, "Not a component type.");

        // Get the component pool.
        ComponentPool<Type>* pool = this->GetPool<Type>();
        Assert(pool != nullptr, "Retrieved a null component pull!");

        // Lookup and return the component.
        return pool->Lookup(handle);
    }

    template<typename Type>
    bool ComponentSystem::Destroy(EntityHandle handle)
    {
        // Validate component type.
        static_assert(std::is_base_of<Component, Type>::value, "Not a component type.");

        // Get the component pool.
        ComponentPool<Type>* pool = this->GetPool<Type>();
        Assert(pool != nullptr, "Retrieved a null component pull!");

        // Destroy the component.
        return pool->Destroy(handle);
    }

    template<typename Type>
    typename ComponentPool<Type>::ComponentIterator ComponentSystem::Begin()
    {
        // Validate component type.
        static_assert(std::is_base_of<Component, Type>::value, "Not a component type.");

        // Get the component pool.
        ComponentPool<Type>* pool = this->GetPool<Type>();
        Assert(pool != nullptr, "Retrieved a null component pull!");

        // Return the iterator.
        return pool->Begin();
    }

    template<typename Type>
    typename ComponentPool<Type>::ComponentIterator ComponentSystem::End()
    {
        // Validate component type.
        static_assert(std::is_base_of<Component, Type>::value, "Not a component type.");

        // Get the component pool.
        ComponentPool<Type>* pool = this->GetPool<Type>();
        Assert(pool != nullptr, "Retrieved a null component pull!");

        // Return the iterator.
        return pool->End();
    }

    template<typename Type>
    ComponentPool<Type>* ComponentSystem::CreatePool()
    {
        // Validate component type.
        static_assert(std::is_base_of<Component, Type>::value, "Not a component type.");

        // Create and add pool to the collection.
        auto pool = std::make_unique<ComponentPool<Type>>();
        auto pair = ComponentPoolPair(typeid(Type), std::move(pool));
        auto result = m_pools.insert(std::move(pair));

        Assert(result.second == true, "Failed to insert a new pool type!");

        // Return the created pool.
        return reinterpret_cast<ComponentPool<Type>*>(result.first->second.get());
    }

    template<typename Type>
    ComponentPool<Type>* ComponentSystem::GetPool()
    {
        // Validate component type.
        static_assert(std::is_base_of<Component, Type>::value, "Not a component type.");

        // Find pool by component type.
        auto it = m_pools.find(typeid(Type));
        if(it == m_pools.end())
        {
            return this->CreatePool<Type>();
        }

        // Cast and return the pointer that we already know is a component pool.
        return reinterpret_cast<ComponentPool<Type>*>(it->second.get());
    }
}
