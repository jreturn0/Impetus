#pragma once
#include <unordered_map>

#include "descriptor/DescriptorAllocatorGrowable.h"
#include "geometry/Mesh.h"
#include "LoadedGLTF.h"
#include "utils/QuickMacros.h"

#include "Forwards.h"
#include "StringHash.h"
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include <vulkan/vulkan_handles.hpp>
#include <shared_mutex>

namespace imp::gfx {

    template<typename T>
    class ResourceCacheBase {
    public:
        using SharedType = std::shared_ptr<T>;
        using WeakType = std::weak_ptr<T>;

        [[nodiscard]] bool add(std::string_view name, SharedType resource) {
            std::unique_lock lock(m_cacheMutex);
            if (m_resources.contains(utl::StringHash(name))) {
                //TODO: maybe throw instead?
                return false;
            }
            m_resources[utl::StringHash(name)] = resource;
            m_resourceNames.push_back(std::string(name));
            return true;
        }


        [[nodiscard]] bool has(utl::StringHash nameHash) const noexcept {
            std::shared_lock lock(m_cacheMutex);
            return m_resources.contains(nameHash);
        }

        // Returns nullptr if not found
        [[nodiscard]] SharedType get(utl::StringHash nameHash) const {
            std::shared_lock lock(m_cacheMutex);
            //TODO: maybe throw instead?
            auto&& it = m_resources.find(nameHash);
            return it != m_resources.end() ? it->second : nullptr;

        }
        // Returns empty weak ptr if not found
        [[nodiscard]] WeakType getWeak(utl::StringHash nameHash) const {
            std::shared_lock lock(m_cacheMutex);
            auto&& it = m_resources.find(nameHash);
            return it != m_resources.end() ? WeakType(it->second) : WeakType{};

        }
        [[nodiscard]] T* getRaw(utl::StringHash nameHash) {
            std::shared_lock lock(m_cacheMutex);
            auto&& it = m_resources.find(nameHash);
            if (it == m_resources.end()) {
                return nullptr;
            }
            return m_resources[nameHash].get();
        }

        SharedType& operator[](utl::StringHash nameHash) {
            std::shared_lock lock(m_cacheMutex);
            auto it = m_resources.find(nameHash);
            if (it == m_resources.end()) {
                Debug::Throw("Resource ( {} ) not found", nameHash.strView);
            }                
            return it->second;
        }

        [[nodiscard]] bool remove(utl::StringHash nameHash) {
            std::unique_lock lock(m_cacheMutex);
            auto&& itRes = m_resources.find(nameHash);
            if (itRes == m_resources.end()) {
                return false;
            }
            m_resources.erase(nameHash);
            //TODO: optimize away from O(n) 
            auto&& itName = std::find_if(m_resourceNames.begin(), m_resourceNames.end(), [&nameHash](const std::string& n) { return utl::StringHash(n) == nameHash; });
            if (itName != m_resourceNames.end())
                std::erase(m_resourceNames, *itName);
            return true;
        }
        [[nodiscard]] const std::vector< std::string>& getNames() const noexcept { std::shared_lock lock(m_cacheMutex); return m_resourceNames; };
    private:
        friend class ResourceCache;
        std::unordered_map <uint64_t, SharedType> m_resources{};
        std::vector<std::string> m_resourceNames{};
        mutable std::shared_mutex m_cacheMutex{};
    };



    class ResourceCache {
    public:
        ResourceCache(const vk::raii::Device& device);
        ResourceCache(const ResourceCache&) = delete;
        ResourceCache& operator=(const ResourceCache&) = delete;
        ResourceCache(ResourceCache&&) = delete;
        ResourceCache& operator=(ResourceCache&&) = delete;;

        auto&& getMaterialDescriptorAllocator() { return m_materialDescriptorAllocator; }

        // Mesh
        [[nodiscard]] bool addMesh(std::string_view name, SharedMesh mesh) { return m_meshCache.add(name, mesh); }
        [[nodiscard]] bool hasMesh(utl::StringHash nameHash) const { return m_meshCache.has(nameHash); }
        [[nodiscard]] SharedMesh getMesh(utl::StringHash nameHash) const { return m_meshCache.get(nameHash); }
        [[nodiscard]] std::weak_ptr<Mesh> getMeshWeak(utl::StringHash nameHash) const { return m_meshCache.getWeak(nameHash); }
        // be careful: will throw if nameHash doesn't exist. Use hasMesh() first to check.
        [[nodiscard]] SharedMesh& getMeshRef(utl::StringHash nameHash) { return m_meshCache[nameHash]; }
        [[nodiscard]] Mesh* getMeshRaw(utl::StringHash nameHash) { return m_meshCache.getRaw(nameHash); }
        [[nodiscard]] bool removeMesh(utl::StringHash nameHash) { return m_meshCache.remove(nameHash); }
        [[nodiscard]] const std::vector<std::string>& getMeshNames() const { return m_meshCache.getNames(); }

        // Image
        [[nodiscard]] bool addImage(std::string_view name, SharedImage image) { return m_imageCache.add(name, image); }
        [[nodiscard]] bool hasImage(utl::StringHash nameHash) const { return m_imageCache.has(nameHash); }
        [[nodiscard]] SharedImage getImage(utl::StringHash nameHash) const { return m_imageCache.get(nameHash); }
        [[nodiscard]] std::weak_ptr<Image> getImageWeak(utl::StringHash nameHash) const { return m_imageCache.getWeak(nameHash); }
        // be careful: will throw if nameHash doesn't exist. Use hasImage() first to check.
        [[nodiscard]] SharedImage& getImageRef(utl::StringHash nameHash) { return m_imageCache[nameHash]; }
        [[nodiscard]] Image* getImageRaw(utl::StringHash nameHash) { return m_imageCache.getRaw(nameHash); }
        [[nodiscard]] bool removeImage(utl::StringHash nameHash) { return m_imageCache.remove(nameHash); }
        [[nodiscard]] const std::vector<std::string>& getImageNames() const { return m_imageCache.getNames(); }

        // Material
        [[nodiscard]] bool addMaterial(std::string_view name, SharedMaterial material) { return m_materialCache.add(name, material); }
        [[nodiscard]] bool hasMaterial(utl::StringHash nameHash) const { return m_materialCache.has(nameHash); }
        [[nodiscard]] SharedMaterial getMaterial(utl::StringHash nameHash) const { return m_materialCache.get(nameHash); }
        [[nodiscard]] std::weak_ptr<Material> getMaterialWeak(utl::StringHash nameHash) const { return m_materialCache.getWeak(nameHash); }
        // be careful: will throw if nameHash doesn't exist. Use hasMaterial() first to check.
        [[nodiscard]] SharedMaterial& getMaterialRef(utl::StringHash nameHash) { return m_materialCache[nameHash]; }
        [[nodiscard]] Material* getMaterialRaw(utl::StringHash nameHash) { return m_materialCache.getRaw(nameHash); }
        [[nodiscard]] bool removeMaterial(utl::StringHash nameHash) { return m_materialCache.remove(nameHash); }
        [[nodiscard]] const std::vector<std::string>& getMaterialNames() const { return m_materialCache.getNames(); }

        // GLTF
        [[nodiscard]] bool addGLTF(std::string_view name, const SharedGLTF& gltf) { return m_gltfCache.add(name, gltf); }
        [[nodiscard]] bool hasGLTF(utl::StringHash nameHash) const { return m_gltfCache.has(nameHash); }
        [[nodiscard]] SharedGLTF getGLTF(utl::StringHash nameHash) const { return m_gltfCache.get(nameHash); }
        [[nodiscard]] std::weak_ptr<LoadedGLTF> getGLTFWeak(utl::StringHash nameHash) const { return m_gltfCache.getWeak(nameHash); }
        // be careful: will throw if nameHash doesn't exist. Use hasGLTF() first to check.
        [[nodiscard]] SharedGLTF& getGLTFRef(utl::StringHash nameHash) { return m_gltfCache[nameHash]; }
        [[nodiscard]] LoadedGLTF* getGLTFRaw(utl::StringHash nameHash) { return m_gltfCache.getRaw(nameHash); }
        [[nodiscard]] bool removeGLTF(utl::StringHash nameHash) { return m_gltfCache.remove(nameHash); }
        [[nodiscard]] const std::vector<std::string>& getGLTFNames() const { return m_gltfCache.getNames(); }


        [[nodiscard]] vk::Sampler getDefaultSamplerLinear() const { return *m_defaultSamplerLinear; }
        [[nodiscard]] vk::Sampler getDefaultSamplerNearest() const { return *m_defaultSamplerNearest; }

    private:
        DescriptorAllocatorGrowable m_materialDescriptorAllocator;

        ResourceCacheBase<Mesh> m_meshCache;
        ResourceCacheBase<Image> m_imageCache;
        ResourceCacheBase<Material> m_materialCache;
        ResourceCacheBase<LoadedGLTF> m_gltfCache;

        //std::vector<vk::raii::Sampler> m_samplerList;
        vk::raii::Sampler m_defaultSamplerLinear;
        vk::raii::Sampler m_defaultSamplerNearest;



    };
}
