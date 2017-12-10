#pragma once

#include "Precompiled.hpp"

/*
    Resource Pool

    Manages a pool for a single type of resources.
    See ResourceManager class for more context.
*/

namespace System
{
    // Resource pool interface.
    class ResourcePoolInterface
    {
    protected:
        ResourcePoolInterface()
        {
        }

    public:
        virtual ~ResourcePoolInterface()
        {
        }

        virtual void ReleaseUnused() = 0;
    };

    // Resource pool class.
    template<typename Type>
    class ResourcePool : public ResourcePoolInterface
    {
    public:
        // Type declarations.
        typedef std::shared_ptr<Type>                        ResourcePtr;
        typedef std::unordered_map<std::string, ResourcePtr> ResourceList;
        typedef typename ResourceList::value_type            ResourceListPair;

    public:
        ResourcePool();
        ~ResourcePool();

        // Sets the default resource.
        void SetDefault(std::shared_ptr<const Type> resource);

        // Gets the default resource.
        std::shared_ptr<const Type> GetDefault() const;

        // Loads a resource from a file.
        std::shared_ptr<const Type> Load(std::string filename);

        // Releases unused resources.
        void ReleaseUnused();

        // Releases all resources.
        void ReleaseAll();

    private:
        // Default resource.
        std::shared_ptr<const Type> m_default;

        // List of resources.
        ResourceList m_resources;
    };

    // Template definitions.
    template<typename Type>
    ResourcePool<Type>::ResourcePool() :
        m_default(std::make_shared<Type>())
    {
    }

    template<typename Type>
    ResourcePool<Type>::~ResourcePool()
    {
        this->ReleaseAll();
    }

    template<typename Type>
    void ResourcePool<Type>::SetDefault(std::shared_ptr<const Type> resource)
    {
        m_default = resource;
    }

    template<typename Type>
    std::shared_ptr<const Type>  ResourcePool<Type>::GetDefault() const
    {
        return m_default;
    }

    template<typename Type>
    std::shared_ptr<const Type> ResourcePool<Type>::Load(std::string filename)
    {
        // Find the resource.
        auto it = m_resources.find(filename);
        if(it != m_resources.end())
            return it->second;

        // Create and load a new resource instance.
        std::shared_ptr<Type> resource = std::make_shared<Type>();
        if(!resource->Load(filename))
            return m_default;

        // Add resource to the list.
        auto result = m_resources.emplace(filename, std::move(resource));
        Assert(result.second, "Failed to emplace a new resource in the resource pool!");

        // Return the resource pointer.
        return result.first->second;
    }

    template<typename Type>
    void ResourcePool<Type>::ReleaseUnused()
    {
        // Release unused resources.
        auto it = m_resources.begin();
        while(it != m_resources.end())
        {
            if(it->second.unique())
            {
                // Save the filename to print it later.
                std::string filename = std::move(it->first);

                // Release the resource.
                it = m_resources.erase(it);

                // Print a log message.
                Log() << "Released a resource loaded from \"" << filename << "\" file.";
            }
            else
            {
                ++it;
            }
        }
    }

    template<typename Type>
    void ResourcePool<Type>::ReleaseAll()
    {
        // Release all resources.
        auto it = m_resources.begin();

        while(it != m_resources.end())
        {
            // Save the filename to print it later.
            std::string filename = std::move(it->first);

            // Release the resource.
            it = m_resources.erase(it);

            // Print a log message.
            Log() << "Released a resource loaded from \"" << filename << "\" file.";
        }

        Assert(m_resources.empty(), "Resource pool is not empty after releasing all resources!");
    }
}
