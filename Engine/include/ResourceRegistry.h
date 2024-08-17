#pragma once
#include <ResourceCache.h>
#include <typeindex>
#include <iostream>
template <typename T>
static constexpr std::type_index GetTypeIndex()
{
	static const std::type_index index = std::type_index(typeid(T));
	return index;
}
class ResourceRegistry
{
private:
	inline static std::unordered_map < std::type_index, std::function<std::unique_ptr<IResourceCache>()>> cacheFactory;

public:

	template<typename Type, typename Loader>
	static void Register()
	{
		cacheFactory[GetTypeIndex<Type>()] = []() {return std::make_unique<ResourceCache<Type, Loader>>(); };
	}

	static std::unordered_map < std::type_index, std::function<std::unique_ptr<IResourceCache>()>>& GetCacheFactory() { return cacheFactory; }
};


#define REGISTER_RESOURCE(Type, Loader)					      \
    namespace {                                               \
        static struct RegisterResource_##Type {                      \
            RegisterResource_##Type() {                       \
				std::cout << "Registering: " << typeid(Type).name() << std::endl; \
                ResourceRegistry::Register<Type, Loader>();  \
            }                                                 \
        } registerResourceInstance_##Type;                    \
    }
