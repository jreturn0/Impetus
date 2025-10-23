#pragma once
#include <entt/entt.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/unordered_map.hpp>
#include <functional>
#include <unordered_map>
#include <string>
#include <type_traits>
#include <iostream>

namespace cereal {
    class JSONOutputArchive;
    class JSONInputArchive;
}

namespace imp {

    namespace componentInfoExt {
        template<typename T>
        struct HookTag {};
    }

    struct ComponentInfo {
        std::string name;
        std::function<void(entt::snapshot&, cereal::JSONOutputArchive&)> serialize;
        std::function<void(entt::snapshot_loader&, cereal::JSONInputArchive&)> deserialize;
    };

    class ComponentInfoRegistry {
    public:
        using ComponentInfoMap = std::unordered_map<entt::id_type, ComponentInfo>;
        template <typename Component>
        static void Register(const char* customName = "") {
            const entt::type_info typeId = entt::type_id<Component>();

            m_infoMap[typeId.hash()] = ComponentInfo{
                (customName && customName[0]) ? customName : std::string(typeId.name()),
                [](entt::snapshot& snap, cereal::JSONOutputArchive& ar) { snap.get<Component>(ar); },
                [](entt::snapshot_loader& loader, cereal::JSONInputArchive& ar) { loader.get<Component>(ar); }
            };
            using componentInfoExt::HookTag;
            if constexpr (requires { OnRegisterComponent(HookTag<Component>{}); }) {
                OnRegisterComponent(HookTag<Component>{});
            }
        }

        template <typename Component>
        static bool IsRegistered() {
            return m_infoMap.contains(entt::type_id<Component>().hash());
        }

        static const ComponentInfoMap& GetComponentInfoMap() {
            return m_infoMap;
        }

        static const ComponentInfoMap& GetInfoMap() {
            return m_infoMap;
        }

    private:
        inline static ComponentInfoMap m_infoMap{};
    };

    template<typename Component>
    struct Registration {
        Registration(const char* customName = "") noexcept {
            imp::ComponentInfoRegistry::Register<Component>(customName);
        }
    };

} // namespace imp

#define REGISTER_COMPONENT(Type) \
  inline static imp::Registration<Type> UNIQUE_NAME(autoRegisterInstance_){}; 

#define REGISTER_COMPONENT_CUSTOM_NAME(Type, Name) \
  inline static imp::Registration<Type> UNIQUE_NAME(autoRegisterInstance_){Name}; 