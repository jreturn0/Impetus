
#include "SceneArchive.h"

#include <ranges>
#include <entt/entity/registry.hpp>


#include "Serialize.h"
#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/common.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/string.hpp>
#include <entt/entity/snapshot.hpp>

#include "AnsiCodes.h"
#include "ComponentInfoRegistry.h"

#include "Debug.h"
#include "SystemManager.h"

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
            snapshot.get<entt::entity>(archive);


            // Serialize all components
            for (auto& [id, info] : ComponentInfoRegistry::GetInfoMap()) {
                std::cout << "Serializing component type: " << id << std::endl;

                try {
                    
                    info.serialize(snapshot, archive);
                } catch ( std::bad_optional_access e) {
                    Debug::Error("Bad optional access for component type: ", std::to_string(id));
                    throw;
                } catch ( std::exception e) {
                    Debug::Error("Exception during component serialization for component type: ", std::to_string(id));
                    throw;
                }
            }
        }

        {// Serializing system manager
            archive(cereal::make_nvp("SystemManager", systemManager));
        }

    } catch (const std::exception& e) {
        Debug::FatalError(e.what());
    }
}

void imp::SceneArchive::Load(entt::registry& registry, SystemManager& systemManager, std::istream& input)
{
    try {
        registry.clear();
        systemManager.clear();

        // Debug: Log number of entities before loading
        size_t entity_count_before = std::distance(registry.view<entt::entity>().begin(), registry.view<entt::entity>().end());
     
        cereal::JSONInputArchive archive(input);
        {
            entt::snapshot_loader loader{ registry };

            // Deserialize all entities
            loader.get<entt::entity>(archive);

            std::set<std::string> successfullyLoadedComponents;
            std::stringstream ss;
            for (auto& info : ComponentInfoRegistry::GetInfoMap() | std::views::values) {
                //std::cout << "Deserializing component type: " << info.name << std::endl;
               // Debug::Info ("Deserializing component type: {}", info.name);
                bool loaded = false;
                try {
                    info.deserialize(loader, archive);
                    loaded = true;
                } catch (std::bad_optional_access e) {
                    Debug::Exception("Bad optional access for component type: {}", info.name);
                    //throw;
                } catch ( std::exception e) {
                    Debug::Exception("During component deserialization for component type: {}:\n\t", info.name,e.what());
                    //throw;
                }
                    ss << (loaded ? ansi::green : ansi::red)<<"\t" << info.name << ansi::reset<< "\n";
            }
            Debug::Info ("Successfully loaded components: \n{}", ss.str());
        }



        {
            archive(cereal::make_nvp("SystemManager", systemManager));
            systemManager.reload();
           // systemManager.initializeSystems(registry);
        }

        // Debug: Log number of entities after loading
        //size_t entity_count_after = std::distance(registry.view<entt::entity>().begin(), registry.view<entt::entity>().end());
        //std::cout << "Number of entities after loading: " << entity_count_after << std::endl;

    } catch (const std::exception& e) {
        Debug::FatalError(e.what());
    }
}
