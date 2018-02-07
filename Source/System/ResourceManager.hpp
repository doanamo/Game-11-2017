#pragma once

#include "Precompiled.hpp"
#include "ResourcePool.hpp"

/*
    Resource Manager

    Tracks resource references and releases them when no longer needed.
    Wraps multiple ResourcePool instances that can hold resources
    of different types in a single ResourceManager instance.

    void ExampleResourceManager()
    {
        // Create a resource manager instance.
        System::ResourceManager resourceManager;
    
        // Set a default resource that will be returned on failed loads.
        std::shared_ptr<Texture> defaultTexture = std::make_shared<Texture>();
        resourceManager->SetDefault<Texture>(defaultTexture);
    
        // Load different resources in a scope.
        {
            // Load a resource from the pool. Returns an existing resource if exist.
            // Resource type must provide Load(std::string, ...) method for the template.
            auto floorTexture = resourceManager->Load<Texture("Data/Textures/floor.png");
            auto grassTexture = resourceManager->Load<Texture>("Data/Textures/grass.png");

            // Load a shader using the same ResourceManager instance.
            std::shared_ptr<Shader> shader = resourceManager->Load<Shader>("Data/Shader/shiny.png");
        }

        // Release resources that are no longer referenced.
        resourceManager.ReleaseUnused();
    }
*/

namespace System
{
    // Resource manager class.
    class ResourceManager
    {
    public:
        // Type declarations.
        typedef std::unique_ptr<ResourcePoolInterface>               ResourcePoolPtr;
        typedef std::unordered_map<std::type_index, ResourcePoolPtr> ResourcePoolList;
        typedef ResourcePoolList::value_type                         ResourcePoolPair;

    public:
        ResourceManager();
        ~ResourceManager();

        // Sets the default resource.
        template<typename Type>
        void SetDefault(std::shared_ptr<const Type> default);

        // Gets the default resource.
        template<typename Type>
        std::shared_ptr<const Type> GetDefault() const;

        // Loads a resource.
        // Name argument does not have to refer to a file path.
        // Resource type must have Load(std::string, ...) method implemented.
        template<typename Type, typename... Arguments>
        std::shared_ptr<const Type> Load(std::string name, Arguments... arguments);

        // Releases unused resources of all types.
        void ReleaseUnused();

        // Gets a resource pool for a specified type of resource.
        template<typename Type>
        ResourcePool<Type>* GetPool();

    private:
        // Creates a resource pool for a specified type of resource.
        template<typename Type>
        ResourcePool<Type>* CreatePool();

    private:
        // Resource pools for different types.
        ResourcePoolList m_pools;
    };

    // Template definitions.
    template<typename Type>
    void ResourceManager::SetDefault(std::shared_ptr<const Type> default)
    {
        // Get a resource pool.
        ResourcePool<Type>* pool = this->GetPool<Type>();
        Assert(pool != nullptr, "Could not retrieve a resource pool!");

        // Set the default resource.
        pool->SetDefault(default);
    }

    template<typename Type>
    std::shared_ptr<const Type> ResourceManager::GetDefault() const
    {
        // Get a resource pool.
        ResourcePool<Type>* pool = this->GetPool<Type>();
        Assert(pool != nullptr, "Could not retrieve a resource pool!");

        // Return the default resource.
        return pool->GetDefault();
    }

    template<typename Type, typename... Arguments>
    std::shared_ptr<const Type> ResourceManager::Load(std::string name, Arguments... arguments)
    {
        // Get a resource pool.
        ResourcePool<Type>* pool = this->GetPool<Type>();
        Assert(pool != nullptr, "Could not retrieve a resource pool!");

        // Delegate call to the resource pool, which will then delegate it further to a new resource instance.
        return pool->Load(name, std::forward<Arguments>(arguments)...);
    }

    template<typename Type>
    ResourcePool<Type>* ResourceManager::CreatePool()
    {
        // Create and add a new resource pool.
        auto pool = std::make_unique<ResourcePool<Type>>();
        auto pair = ResourcePoolPair(typeid(Type), std::move(pool));
        auto result = m_pools.insert(std::move(pair));
        Assert(result.second, "Could not insert a new resource pool!");

        // Return the created resource pool.
        return reinterpret_cast<ResourcePool<Type>*>(result.first->second.get());
    }

    template<typename Type>
    ResourcePool<Type>* ResourceManager::GetPool()
    {
        // Find a pool by resource type.
        auto it = m_pools.find(typeid(Type));
        if(it != m_pools.end())
        {
            // Cast and return the pointer that we already know is a resource pool.
            Assert(it->second.get() != nullptr, "Retrieved resource pool is null!");
            return reinterpret_cast<ResourcePool<Type>*>(it->second.get());
        }
        else
        {
            // Create and return a new resource pool.
            return this->CreatePool<Type>();
        }
    }
};
