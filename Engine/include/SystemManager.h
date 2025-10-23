#pragma once
#include "Debug.h"
#include "EngineStats.h"
#include "events/BasicEvents.h"
#include "StringUtils.h"
#include "System.h"
#include "SystemRegistry.h"
#include "UpdateType.h"
#include <cereal/cereal.hpp>
#include <cereal/types/unordered_map.hpp>
#include <map>
#include <string_view>

namespace imp {
    class Clock;
    class SystemManager
    {

        struct Entry
        {
            std::unique_ptr<System> system;
            bool status{ false };
            std::string name;

            template <class Archive>
            void serialize(Archive& ar)
            {
                ar(status, name);
            }
        };
        struct Meta
        {
            size_t index;
            UpdateType type;
            template <class Archive>
            void serialize(Archive& ar)
            {
                ar(index, type);
            }
        };
    public:

        void OnSystemReorderEvent(SystemReorderEvent& event);
        void OnSystemAddEvent(AddSystemEvent& event);
        void OnSystemRemoveEvent(RemoveSystemEvent& event);

        template <class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::make_nvp("systems", m_systems),
                cereal::make_nvp("preFixedSystems", m_preFixedSystems),
                cereal::make_nvp("fixedSystems", m_fixedSystems),
                cereal::make_nvp("postFixedSystems", m_postFixedSystems),
                cereal::make_nvp("systemMetaMap", m_systemMetaMap));
        }

        void addSystem(const std::string_view name, bool enabled = true, UpdateType type = UpdateType::Variable);
        void initializeSystems(entt::registry& registry);
        void updateSystems(entt::registry& registry, Clock& time, EngineStats& stats);
        void cleanupSystems(entt::registry& registry) const;
        void setSystemStatus(const std::string_view name, bool status);
        void onSystemStatusEvent(const SystemStatusEvent& statusEvent);
        void clear();
        void clearAndCleanup(entt::registry& registry);
        void reload();
    private:
        std::vector<Entry> m_systems{};
        std::vector<Entry> m_preFixedSystems{};
        std::vector<Entry> m_fixedSystems{};
        std::vector<Entry> m_postFixedSystems{};
        std::unordered_map<std::string, Meta> m_systemMetaMap{};
        std::queue<std::function<void(entt::registry&)>> m_pendingEvents{};
        bool m_reorderDispatchPending{ true };

        std::vector<Entry>& getContainer(const UpdateType type);

    };



}
