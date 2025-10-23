#pragma once
#include "ComponentInfoRegistry.h"
#include "entt/entity/entity.hpp"
#include "entt/entt.hpp"
#include "RegisterMacro.h"
#include "TypeWidgets.h"
#include <boost/pfr.hpp>

#include <concepts>
#include <functional>
#include <string>
#include <type_traits>
#include <unordered_map>
#ifdef _DEBUG
#include <imgui.h>
#endif

namespace imp {
    namespace editorDetail {
        template<typename T>
        struct IsEnttTag : std::false_type {};
        //TODO: fix, things seemingly work but the error is here anyway
        template<entt::id_type Value>
        struct IsEnttTag<entt::tag<Value>> : std::true_type {};


        template<typename T>
        inline constexpr bool IsEnttTagV = IsEnttTag<T>::value;



        template <typename Component>
        concept ValidComponent = std::is_aggregate_v<Component> && !IsEnttTagV<Component>;

        template<typename T>
        struct IsComparable {
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
        inline constexpr bool IsPfrIntrospectableV = IsPfrIntrospectable<T>::value;
    }

    namespace componentUtils {

        // Concept to check if a type is a valid component for the editor



        //template <class Component, std::enable_if_t<std::is_aggregate_v<Component> && !editorDetail::IsEnttTag<Component>::value, int> = 0>
        template <typename Component>
            requires std::is_default_constructible_v<Component>
        void Widget(entt::registry& registry, entt::entity entity)
        {
            static auto typeInfo = entt::type_id<Component>();

            if constexpr (editorDetail::IsEnttTagV<Component>) {
                if (registry.any_of<Component>(entity)) {
                    ImGui::Text("%s (tag)", typeInfo.name().data());
                }
                else {
                    ImGui::Text("No tag of this type.");
                }
                return;
            }
            else {
                if (!registry.all_of<Component>(entity)) {
                    ImGui::Text("No component of this type.");
                    return;
                }
                if constexpr (!editorDetail::IsPfrIntrospectableV<Component>) {
                    ImGui::Text("%s", typeInfo.name().data());
                    return;
                }
                else {
                    ImGui::Text("%s", typeInfo.name().data());

                    Component& comp = registry.get<Component>(entity);

                    // Use PFR to get field names and iterate over fields
                    constexpr auto field_names = boost::pfr::names_as_array<Component>();
                    size_t idx = 0;
                    boost::pfr::for_each_field(comp, [&](auto& field) {
                        std::string fieldName = std::string(field_names[idx]);
                        using FieldType = std::decay_t<decltype(field)>;
                        auto originalField = field;
                        TypeWidget(field, fieldName);

                        if constexpr (editorDetail::IsComparable<FieldType>::value) {
                            if (field != originalField) {
                                registry.patch<Component>(entity, [&](Component& c) { c = comp; });
                            }
                        }
                        ++idx;
                    });
                }
            }
        }



        template <typename Component>
            requires std::is_default_constructible_v<Component>
        void AddAction(entt::registry& registry, entt::entity entity)
        {
            if (!registry.all_of<Component>(entity)) {
                registry.emplace<Component>(entity);
            }
        }

        template <typename Component>
            requires std::is_default_constructible_v<Component>
        void RemoveAction(entt::registry& registry, entt::entity entity)
        {
            if (registry.all_of<Component>(entity)) {
                registry.remove<Component>(entity);
            }
        }
    }



    struct EditorComponentInfo {
        using EntityCallback = std::function<void(entt::registry&, entt::entity)>;
        EntityCallback widget;
        EntityCallback create;
        EntityCallback destroy;
    };

    class EditorComponentRegistry {
    public:
        static auto& EditorInfo() {
            static std::unordered_map<entt::id_type, EditorComponentInfo> instance;
            return instance;
        }

        template <typename Component>
        static void OnComponentRegistered() {
            auto typeId = entt::type_id<Component>();
            EditorInfo()[typeId.hash()] = EditorComponentInfo{
                [](entt::registry& r, entt::entity e) { imp::componentUtils::Widget<Component>(r, e); },
                [](entt::registry& r, entt::entity e) { imp::componentUtils::AddAction<Component>(r, e); },
                [](entt::registry& r, entt::entity e) { imp::componentUtils::RemoveAction<Component>(r, e); }
            };
            Debug::Info("Registered editor component info for type: {}", typeId.name());
        }

        // Optional: method to add custom widget
        static void AddCustomWidget(entt::id_type typeId, EditorComponentInfo::EntityCallback widgetFunc) {
            auto& editorInfo = EditorInfo();
            if (editorInfo.find(typeId) != editorInfo.end()) {
                editorInfo[typeId].widget = std::move(widgetFunc);
            }
            else {
                Debug::Warning("Adding custom widget for unregistered component type id: {}", typeId);
                editorInfo[typeId] = EditorComponentInfo{ std::move(widgetFunc), nullptr, nullptr };
            }
        }

        // Convenience: typed overload
        template<typename Component>
        static void AddCustomWidget(EditorComponentInfo::EntityCallback widgetFunc) {
            AddCustomWidget(entt::type_id<Component>().hash(), std::move(widgetFunc));
        }


    };




    namespace componentInfoExt {
        template<typename T>
        void OnRegisterComponent(HookTag<T>) {   
            EditorComponentRegistry::OnComponentRegistered<T>();
        }
    }

    template<typename T>
    struct CustomWidget {
        CustomWidget(EditorComponentInfo::EntityCallback widgetFunc) noexcept {
            imp::EditorComponentRegistry::AddCustomWidget(entt::type_id<T>().hash(), widgetFunc);
        }
    };


} // namespace imp


// Macro add custom widget
#define REGISTER_CUSTOM_COMPONENT_WIDGET(Type, Func) \
    inline static imp::CustomWidget<Type> UNIQUE_NAME(autoRegisterInstance_){Func};

