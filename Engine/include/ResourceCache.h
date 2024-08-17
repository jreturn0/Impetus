#pragma once
#include <entt/resource/cache.hpp>
#include <ImpTypes.h>

class IResourceCache
{
public:
	virtual ~IResourceCache() = default;
	virtual bool load(ID id, const char* path) = 0;
	virtual bool erase(ID id) = 0;
};
template<typename Resource>
class ITypedResourceCache : public IResourceCache
{
public:
	virtual entt::resource<Resource> get(ID id) = 0;
};

template<typename Resource, typename Loader>
class ResourceCache : public ITypedResourceCache<Resource>
{
public:
	entt::resource_cache<Resource, Loader> cache;

	bool load(ID id, const char* path) override
	{
		auto loaded = cache.load(id, path);
		return loaded.second;
	}

	entt::resource<Resource> get(ID id) override
	{
		return cache[id];
	}

	bool erase(ID id) override
	{
		return cache.erase(id);
	}
};
