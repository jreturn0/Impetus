#pragma once
#include <boost/pfr.hpp>
#include <entt/core/type_info.hpp>
#include <entt/entity/snapshot.hpp>
#include <entt/entt.hpp>
#include <functional>
#include <string>
#include "RegisterMacro.h"
#ifdef _DEBUG
#include "DebugUtils/DebugGuiTypeWidgets.h"
#endif

namespace cereal {
    class JSONOutputArchive;
    class JSONInputArchive;
}

namespace cereal {
    class JSONOutputArchive;
    class JSONInputArchive;
}

template<typename T>
struct IsComparable
{
private:
    template<typename U>
    static auto test(int) -> decltype(std::declval<U>() != std::declval<U>(), std::true_type());

    template<typename>
    static std::false_type test(...);

public:
    static constexpr bool value = decltype(test<T>(0))::value;
};

// Helper trait to check if a type is introspectable with boost::pfr
template <typename T, typename = void>
struct IsPfrIntrospectable : std::false_type {};

template <typename T>
struct IsPfrIntrospectable < T, std::void_t<decltype(boost::pfr::for_each_field(std::declval<T&>(), [](auto&) {})) >> : std::true_type {};

template <typename T>
struct IsEnttTag : std::false_type {};

template <entt::id_type Value>
struct IsEnttTag<entt::tag<Value>> : std::true_type {};

namespace imp {
    template <class Component, std::enable_if_t<std::is_aggregate_v<Component> && !IsEnttTag<Component>::value, int> = 0>
    void ComponentWidget(entt::registry& registry, entt::entity entity)
    {
#ifdef _DEBUG
        if (registry.all_of<Component>(entity)) {
            auto& comp = registry.get<Component>(entity);
            auto typenameStr = typeid(comp).name();
            ImGui::Text("%s", typenameStr);

            // Use PFR to get field names and iterate over fields
            constexpr auto field_names = boost::pfr::names_as_array<Component>();
            size_t idx = 0;
            boost::pfr::for_each_field(comp, [&](auto& field) {
                std::string fieldName = std::string(field_names[idx]);
                using FieldType = std::decay_t<decltype(field)>;

                auto originalField = field;
                TypeWidget(field, fieldName);

                if constexpr (IsComparable<FieldType>::value) {
                    if (field != originalField) {
                        registry.patch<Component>(entity, [&](Component& c) {
                            c = comp;
                                                  });
                    }
                }

                ++idx;
                                       });
        } else {
            ImGui::Text("No component of this type.");
        }
#endif 
    }

    // Fallback function for non-aggregate or tag types
    template <class Component, std::enable_if_t<!std::is_aggregate_v<Component> || IsEnttTag<Component>::value, int> = 0>
    void ComponentWidget(entt::registry & registry, entt::entity entity)
    {
        // No-op
    }


    template <typename Component>
    void ComponentAddAction(entt::registry& registry, entt::entity entity) { registry.template emplace<Component>(entity); }
    template <typename Component>
    void ComponentRemoveAction(entt::registry& registry, entt::entity entity) { registry.template remove<Component>(entity); }
    template <typename Component>
    void ComponentSerialize(entt::snapshot& snapshot, cereal::JSONOutputArchive& archive) { snapshot.template get<Component>(archive); };
    template <typename Component>
    void ComponentDeserialize(entt::snapshot_loader& snapshot, cereal::JSONInputArchive& archive) { snapshot.template get<Component>(archive); };
    struct ComponentInfo
    {
        using SerializeCallback = std::function<void(entt::snapshot&, cereal::JSONOutputArchive&)>;
        using DeserializeCallback = std::function<void(entt::snapshot_loader&, cereal::JSONInputArchive&)>;
        using WidgetCallback = std::function<void(entt::registry&, entt::entity)>;
        std::string name;
        WidgetCallback widget, create, destroy;
        SerializeCallback serialize;
        DeserializeCallback deserialize;
    };
    template<typename T>
    constexpr entt::id_type TypeHash()
    {
        return entt::type_id<T>().hash();
    }
    class ComponentInfoRegistry
    {
        inline static std::unordered_map<entt::id_type, ComponentInfo> infoMap{};
    public:

        inline static std::unordered_map<entt::id_type, ComponentInfo>& GetInfoMap() { return infoMap; };
        template <typename Component>
        inline static void RegisterComponent(const char* customName = "")
        {
            infoMap[TypeHash<Component>()] = ComponentInfo{
                customName == "" ? typeid(Component).name() : customName,
                ComponentWidget<Component>,
                ComponentAddAction<Component>,
                ComponentRemoveAction<Component>,
                ComponentSerialize<Component>,
                ComponentDeserialize<Component>
            };
        }
        template <typename Component>
        inline static void RegisterComponent(ComponentInfo::WidgetCallback customWidget, const char* customName = "")
        {
            infoMap[TypeHash<Component>()] = ComponentInfo{
                customName == "" ? typeid(Component).name() : customName,
                customWidget,
                ComponentAddAction<Component>,
                ComponentRemoveAction<Component>,
                ComponentSerialize<Component>,
                ComponentDeserialize<Component>
            };
        }
    };

    namespace Register {
        template<typename T>
        struct ComponentRegistrar
        {
            ComponentRegistrar(const char* customName = "")
            {
                imp::ComponentInfoRegistry::RegisterComponent<T>(customName);
            }

            explicit ComponentRegistrar(const ComponentInfo::WidgetCallback& customWidget, const char* customName = "")
            {
                imp::ComponentInfoRegistry::RegisterComponent<T>(customWidget, customName);
            }
        };
    }
}
#define REGISTER_COMPONENT(Type) \
    inline static imp::Register::ComponentRegistrar<Type> UNIQUE_NAME(autoRegisterInstance_){};

#define REGISTER_COMPONENT_CUSTOM_WIDGET(Type, CustomWidget) \
    inline static imp::Register::ComponentRegistrar<Type> UNIQUE_NAME(autoRegisterInstance_){CustomWidget};
#define REGISTER_COMPONENT_CUSTOM_NAME(Type,Name) \
    inline static imp::Register::ComponentRegistrar<Type> UNIQUE_NAME(autoRegisterInstance_){Name};
#define REGISTER_COMPONENT_CUSTOM_WIDGET_AND_NAME(Type, CustomWidget,Name) \
    inline static imp::Register::ComponentRegistrar<Type> UNIQUE_NAME(autoRegisterInstance_){CustomWidget,Name};




