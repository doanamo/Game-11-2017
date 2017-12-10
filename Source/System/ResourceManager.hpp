#pragma once

#include "Precompiled.hpp"
#include "ResourcePool.hpp"

/*
    Resource Manager

    Tracks resource references and releases them when no longer needed.

    Example usage:
        System::ResourceManager resourceManager;
    
        auto instanceDefault = std::make_shared<Class>();
        resourceManager->SetDefault<Class>(instanceDefault);
    
        {
            auto instanceA = resourceManager->Load<Class>("Resources/InstanceA");
            auto instanceB = resourceManager->Load<Class>("Resources/InstanceB");
        }
    
        resourceManager.ReleaseUnused();
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
        template<typename Type>
        std::shared_ptr<const Type> Load(std::string filename);

        // Releases unused resources.
        void ReleaseUnused();

        // Gets a resource pool.
        template<typename Type>
        ResourcePool<Type>* GetPool();

    private:
        // Creates a resource pool.
        template<typename Type>
        ResourcePool<Type>* CreatePool();

    private:
        // Resource pools.
        ResourcePoolList m_pools;
    };

    // Template definitions.
    template<typename Type>
    void ResourceManager::SetDefault(std::shared_ptr<const Type> default)
    {
        // Get the resource pool.
        ResourcePool<Type>* pool = this->GetPool<Type>();
        Assert(pool != nullptr, "Could not retrieve a resource pool!");

        // Set the default resource.
        pool->SetDefault(default);
    }

    template<typename Type>
    std::shared_ptr<const Type> ResourceManager::GetDefault() const
    {
        // Get the resource pool.
        ResourcePool<Type>* pool = this->GetPool<Type>();
        Assert(pool != nullptr, "Could not retrieve a resource pool!");

        // Return the default resource.
        return pool->GetDefault();
    }

    template<typename Type>
    std::shared_ptr<const Type> ResourceManager::Load(std::string filename)
    {
        // Get the resource pool.
        ResourcePool<Type>* pool = this->GetPool<Type>();
        Assert(pool != nullptr, "Could not retrieve a resource pool!");

        // Delegate to the resource pool.
        return pool->Load(filename);
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
        // Find the pool by resource type.
        auto it = m_pools.find(typeid(Type));

        if(it != m_pools.end())
        {
            // Cast and return the pointer that we already know is a resource pool.
            return reinterpret_cast<ResourcePool<Type>*>(it->second.get());
        }

        // Create and return a new resource pool.
        return this->CreatePool<Type>();
    }
};
