#include "AnsiCodes.h"
#include "ComponentInfoRegistry.h"
#include "CoreSerialize.h"
#include "Debug.h"
#include "SceneArchive.h"
#include "SystemManager.h"
#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/common.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/vector.hpp>
#include <entt/entity/registry.hpp>
#include <entt/entity/snapshot.hpp>
#include <ranges>
namespace {
    template<typename F>
    struct CerealNode {
        F fn;
        template <class Archive>
        void serialize(Archive& ar) {
            fn(ar);
        }
    };
}
bool imp::SceneArchive::EntityHasComponent(entt::entity& entity, entt::registry& registry, entt::id_type component)
{
    const auto* storage_ptr = registry.storage(component);
    return storage_ptr != nullptr && storage_ptr->contains(entity);
}
void imp::SceneArchive::Save(entt::registry& registry, SystemManager& systemManager, std::ostream& output)
{
    try {
        cereal::JSONOutputArchive archive(output);

        {// Serializing entt registry
            entt::snapshot snapshot{ registry };

            // Serialize all entities
            archive(cereal::make_nvp("Entities",
                CerealNode{ [&](auto& ar) { snapshot.get<entt::entity>(ar); } }
            ));


            // Gather components and sort by name for consistent ordering
            std::vector<std::pair<entt::id_type, const imp::ComponentInfo*>> components;
            components.reserve(imp::ComponentInfoRegistry::GetInfoMap().size());
            for (auto& [id, info] : imp::ComponentInfoRegistry::GetInfoMap()) {
                components.emplace_back(id, &info);
            }
            std::ranges::sort(components, {}, [](const auto& p) { return p.second->name; });

            // Serialize each component type
            for (auto& [id, info] : components) {
                Debug::Info("Serializing component: {}", info->name);                
                try {
                    archive(cereal::make_nvp(info->name.c_str(),
                        CerealNode{ [&](auto& ar) { info->serialize(snapshot, ar); } }
                    ));
                }
                catch (const std::bad_optional_access& e) {
                    Debug::Error("Bad optional access for component type: ", std::to_string(id));
                    throw;
                }
                catch (const std::exception& e) {
                    Debug::Error("Exception during component serialization for component type: ", std::to_string(id));
                    throw;
                }
            }
        }

        {// Serializing system manager
            archive(cereal::make_nvp("SystemManager", systemManager));
        }

    }
    catch (const std::exception& e) {
        Debug::FatalError(e.what());
    }
}

void imp::SceneArchive::Load(entt::registry& registry, SystemManager& systemManager, std::istream& input)
{
    try {
        registry.clear();
        systemManager.clear();

        size_t entity_count_before = std::distance(registry.view<entt::entity>().begin(), registry.view<entt::entity>().end());

        cereal::JSONInputArchive archive(input);
        {
            entt::snapshot_loader loader{ registry };


            try {
                archive(cereal::make_nvp("Entities",
                    CerealNode{ [&](auto& ar) { loader.get<entt::entity>(ar); } }
                ));
            }
            catch (const std::exception& e) {
                Debug::Exception("Failed loading entities: {}", e.what());
                return;
            }

            // Gather components and sort by name for consistent ordering
            std::vector<const imp::ComponentInfo*> components;
            components.reserve(imp::ComponentInfoRegistry::GetInfoMap().size());
            for (auto& kv : imp::ComponentInfoRegistry::GetInfoMap() | std::views::values) {
                components.emplace_back(&kv);
            }
            std::ranges::sort(components, {}, [](const imp::ComponentInfo* ci) { return ci->name; });

            std::set<std::pair<bool, std::string_view>> componentLoadStatus{};

            for (auto* info : components) {
                
                bool loaded = false;
                try {
                    // Each component is looked up by name; missing nodes are tolerated via exception handling
                    archive(cereal::make_nvp(info->name.c_str(),
                        CerealNode{ [&](auto& ar) { info->deserialize(loader, ar); } }
                    ));
                    loaded = true;
                }
                catch (const std::bad_optional_access&) {
                    Debug::Exception("Bad optional access for component type: {}", info->name);
                }
                catch (const std::exception& e) {
                    Debug::Exception("During component deserialization for component type: {}:\n\t{}", info->name, e.what());
                }                
                componentLoadStatus.emplace(loaded, info->name);
            }
            for (const auto& [loaded, name] : componentLoadStatus) {
                if (loaded) {
                    Debug::Info("Loaded component: {}", name);
                }
                else {
                    Debug::Info("Failed to load component: {}", name);
                }
            }
        }

        {
            archive(cereal::make_nvp("SystemManager", systemManager));
            systemManager.reload();
        }                      
    }
    catch (const std::exception& e) {
        Debug::FatalError(e.what());
    }
}
