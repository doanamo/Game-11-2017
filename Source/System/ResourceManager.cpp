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
        auto& pool = pair.second;
        pool->ReleaseUnused();
    }
}
