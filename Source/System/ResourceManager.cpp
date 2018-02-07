#include "Precompiled.hpp"
#include "ResourceManager.hpp"
using namespace System;

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
}

void ResourceManager::ReleaseUnused()
{
    // Release all unused resources.
    for(auto& pair : m_pools)
    {
        Assert(pair.second != nullptr, "Resource pool is null!");

        // Release unused resources from each pool.
        auto& pool = pair.second;
        pool->ReleaseUnused();
    }
}
