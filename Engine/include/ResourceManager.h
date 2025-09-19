#pragma once
#include <string>
#include <ResourceRegistry.h>
#include <ImpTypes.h>
#include <stdexcept>
#include <Debug.h>
#include <ResourceMember.h>
namespace imp {


	class ResourceManager
	{
	private:
		ID idCounter = 0;
		std::unordered_map<std::string, ID> pathIDMap;
		std::unordered_map<std::type_index, std::unique_ptr<IResourceCache>> cacheMap;
		ID GetPathID(const std::string& path);

	public:
		template<typename Type>
		bool load(const std::string& path);

		template<typename Type>
		entt::resource<Type> get(const std::string& path);

		template<typename Type>
		std::pair<entt::resource<Type>, bool> loadAndGet(const std::string& path);

		template<typename Type>
		void loadMember(ResourceMember<Type>& member)
		{
			member.handle = loadAndGet<Type>(member.path).first;
			member.loaded = true;
		}
		template<typename Type>
		ResourceMember<Type> createMember(const std::string& path, ResourceType type)
		{
			ResourceMember<Type> member;
			member.handle = loadAndGet<Type>(path).first;
			member.type = type;
			member.loaded = true;
			return member;
		}
		//TODO: Method to create and/or load ResourceMember


	};




	template<typename Type>
	inline bool ResourceManager::load(const std::string& path)
	{
		auto typeIndex = GetTypeIndex<Type>();
		auto& cacheFactory = ResourceRegistry::GetCacheFactory();

		if (cacheMap.find(typeIndex) == cacheMap.end()) {
			if (cacheFactory.find(typeIndex) == cacheFactory.end()) {
				throw std::runtime_error("Type not registered.");
			}
			cacheMap[typeIndex] = cacheFactory[typeIndex]();
		}
		auto id = GetPathID(path);
		return cacheMap[typeIndex]->load(id, path.c_str());
	}

	template<typename Type>
	inline entt::resource<Type> ResourceManager::get(const std::string& path)
	{
		auto typeIndex = GetTypeIndex<Type>();
		if (cacheMap.find(typeIndex) == cacheMap.end()) {
			throw std::runtime_error("Resource not loaded.");
		}

		auto typedCache = dynamic_cast<ITypedResourceCache<Type>*>(cacheMap[typeIndex].get());
		if (!typedCache) {
			throw std::runtime_error("Failed to cast to the correct cache type.");
		}

		return typedCache->get(GetPathID(path));
	}

	template<typename Type>
	inline std::pair<entt::resource<Type>, bool> ResourceManager::loadAndGet(const std::string& path)
	{
		auto&& inserted = load<Type>(path);
		auto&& handle = get<Type>(path);
		return std::make_pair(handle, inserted);
	}
}
		//auto typeIndex = GetTypeIndex<Type>();
		//auto&& it = cacheMap.find(typeIndex);
		//if (it == cacheMap.end()) {
		//	auto& cacheFactory = ResourceRegistry::GetCacheFactory();
		//	if (cacheFactory.find(typeIndex) == cacheFactory.end()) {
		//		throw std::runtime_error("Type not registered.");
		//	}
		//	cacheMap[typeIndex] = cacheFactory[typeIndex]();
		//}
		//auto&&  typedCache = dynamic_cast<ITypedResourceCache<Type>*>(cacheMap[typeIndex].get());
		//if (!typedCache) {
		//	throw std::runtime_error("Failed to cast to the correct cache type.");
		//}
		//auto id = GetPathID(path);
		//bool inserted = typedCache->load(id, path.c_str());



		//return std::make_pair(typedCache->get(id), inserted);
	


