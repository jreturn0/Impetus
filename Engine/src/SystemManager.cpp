#include "Clock.h"
#include "CtxRef.h"
#include "EngineStats.h"
#include "SystemManager.h"
#include "SystemRegistry.h"
#include <Debug.h>
#include <iostream>
#include <ranges>


namespace {
    static constexpr std::string_view ToString(const imp::UpdateType type)
    {
        switch (type) {
        case imp::Variable:
            return "Variable";
        case imp::Fixed:
            return "Fixed";
        case imp::PreFixed:
            return "PreFixed:";
        case imp::PostFixed:
            return "PostFixed";
        default: 			return "Invalid";
        }
    }
}


std::vector<imp::SystemManager::Entry>& imp::SystemManager::getContainer(const UpdateType type)
{
    switch (type) {
    case Variable:
        return m_systems;
    case Fixed:
        return m_fixedSystems;
    case PreFixed:
        return m_preFixedSystems;
    case PostFixed:
        return m_postFixedSystems;
    default:
        Debug::Throw("Invalid System UpdateType: {}", static_cast<size_t>(type));
        return m_systems;
    }
}

void imp::SystemManager::OnSystemReorderEvent(SystemReorderEvent& event)
{
    if (!m_systemMetaMap.contains(event.name)) {
        Debug::Error("System not found: {}", event.name);
        return;
    }
    m_pendingEvents.emplace([this, event](entt::registry&) {
        auto&& it = m_systemMetaMap.find(event.name);
        if (it == m_systemMetaMap.end()) {
            Debug::Error("System not found: {}", event.name);
            return;
        }
        auto&& [name, meta] = *it;
        auto&& container = getContainer(meta.type);
        Entry system = std::move(container[meta.index]);
        const auto currentIndex = meta.index;
        const auto newIndex = event.newIndex;
        const auto currentType = meta.type;
        const auto newType = event.type;
        Debug::Info("Moving system {} from {} {} to {} {}", name, ToString(currentType), currentIndex, ToString(newType), newIndex);
        std::this_thread::sleep_for(std::chrono::microseconds(10));
        if (newType != currentType) {
            auto&& newContainer = getContainer(event.type);
            container.erase(container.begin() + static_cast<long long>(currentIndex));
            meta.index = newIndex;
            meta.type = newType;
            if (newIndex >= newContainer.size()) {
                newContainer.emplace_back(std::move(system));
                return;
            }
            else {
                newContainer.insert(newContainer.begin() + static_cast<long long>(newIndex), std::move(system));
            }
            for (auto i = 0; i < container.size(); i++) {
                m_systemMetaMap[container[i].name].index = i;
            }
            for (auto i = 0; i < newContainer.size(); i++) {
                m_systemMetaMap[newContainer[i].name].index = i;
            }
            return;
        }
        if (event.newIndex >= container.size()) {
            Debug::Error("Invalid system index");
            return;
        }
        container.erase(container.begin() + static_cast<long long>(currentIndex));


        meta.index = newIndex;
        container.insert(container.begin() + static_cast<long long>(newIndex), std::move(system));

        for (size_t i = 0; i < container.size(); i++) {
            m_systemMetaMap[container[i].name].index = i;
        }
        });



}

void imp::SystemManager::OnSystemAddEvent(AddSystemEvent& event)
{
    m_pendingEvents.emplace([this, event](entt::registry& registry) {
        addSystem(event.name, false, event.type);
        getContainer(event.type)[m_systemMetaMap[event.name].index].system->initialize(registry);
        Debug::Info("System added: {}", event.name);
        });
    m_reorderDispatchPending = true;
}

void imp::SystemManager::OnSystemRemoveEvent(RemoveSystemEvent& event)
{
    auto it = m_systemMetaMap.find(event.name);
    if (it != m_systemMetaMap.end()) {
        auto&& [name, meta] = *it;
        auto&& container = getContainer(meta.type);
        container.erase(container.begin() + meta.index);
        m_systemMetaMap.erase(it);
        for (auto i = 0; i < container.size(); i++) {
            m_systemMetaMap[container[i].name].index = i;
        }
        m_reorderDispatchPending = true;
    }
    else {
        Debug::Error("System not found: {}", event.name);
    }
}


void imp::SystemManager::addSystem(const std::string_view name, bool enabled, UpdateType type)
{
    auto&& systemFactory = SystemRegistry::GetSystemFactory();
    if (const auto it = systemFactory.find(name); it != systemFactory.end()) {
        auto nameStr = std::string(name);
        Entry entry = { it->second(), enabled , nameStr };
        auto&& container = getContainer(type);
        container.push_back(std::move(entry));
        m_systemMetaMap[nameStr] = { container.size() - 1, type };

        m_reorderDispatchPending = true;
    }
    else {
        Debug::Error("System not registered: {}", name);
    }
}

void imp::SystemManager::initializeSystems(entt::registry& registry)
{
    
        for (auto&& entry : m_systems) {
            entry.system->initialize(registry);
        }
        for (auto&& entry : m_preFixedSystems) {
            entry.system->initialize(registry);
        }
        for (auto&& entry : m_fixedSystems) {
            entry.system->initialize(registry);
        }
        for (auto&& entry : m_postFixedSystems) {
            entry.system->initialize(registry);
        }
    

    registry.ctx().get<CtxRef<entt::dispatcher>>().get().sink<SystemReorderEvent>().connect<&SystemManager::OnSystemReorderEvent>(*this);
    registry.ctx().get<CtxRef<entt::dispatcher>>().get().sink<AddSystemEvent>().connect<&SystemManager::OnSystemAddEvent>(*this);
    registry.ctx().get<CtxRef<entt::dispatcher>>().get().sink<RemoveSystemEvent>().connect<&SystemManager::OnSystemRemoveEvent>(*this);
    m_reorderDispatchPending = true;
}

void imp::SystemManager::updateSystems(entt::registry& registry, Clock& time, EngineStats& stats)
{

    const double deltaTime = time.getDelta();
    while (!m_pendingEvents.empty()) {
        m_pendingEvents.front()(registry);
        m_pendingEvents.pop();
    }
    if (m_reorderDispatchPending) {
        SystemsReorderedEvent reorderedEvent;
        reorderedEvent.variable.reserve(m_systems.size());
        for (auto&& [system, status, name] : m_systems) {
            reorderedEvent.variable.emplace_back(name, status);
            Debug::Info("Reorder Variable System ( name: {}, status: {} )", name, status);

        }
        reorderedEvent.preFixed.reserve(m_systems.size());
        for (auto&& [system, status, name] : m_preFixedSystems) {
            reorderedEvent.preFixed.emplace_back(name, status);
            Debug::Info("Reorder PreFixed System ( name: {}, status: {} )", name, status);
        }
        reorderedEvent.fixed.reserve(m_systems.size());
        for (auto&& [system, status, name] : m_fixedSystems) {
            reorderedEvent.fixed.emplace_back(name, status);
            Debug::Info("Reorder Fixed System ( name: {}, status: {} )", name, status);
        }
        reorderedEvent.postFixed.reserve(m_systems.size());
        for (auto&& [system, status, name] : m_postFixedSystems) {
            reorderedEvent.postFixed.emplace_back(name, status);
            Debug::Info("Reorder PostFixed System ( name: {}, status: {} )", name, status);
        }

        registry.ctx().get<CtxRef<entt::dispatcher>>().get().enqueue<SystemsReorderedEvent>(std::move(reorderedEvent));
        m_reorderDispatchPending = false;
    }

    static auto& factory = SystemRegistry::GetSystemFactory();
    for (auto& [system, status, name] : m_systems) {
        if (status) {
            auto&& it = factory.find(utl::StringHash(name));
            ENGINESTATS_SCOPED_TIMER_STR(stats, it->first);
            system->update(registry, static_cast<float>(deltaTime));
        }
    }
    if (time.isFixed()) {
        const float fixedStep = static_cast<float>(time.getFixedStep());
        //Debug::Info("FixedStep {}", fixedStep);

        for (auto& [system, status, name] : m_preFixedSystems) {
            if (status) {
                auto&& it = factory.find(utl::StringHash(name));
                ENGINESTATS_SCOPED_TIMER_STR(stats, it->first);
                system->update(registry, fixedStep);
            }
        }
        while (time.fixedUpdate()) {
            for (auto& [system, status, name] : m_fixedSystems) {
                if (status) {
                    auto&& it = factory.find(utl::StringHash(name));
                    ENGINESTATS_SCOPED_TIMER_STR(stats, it->first);
                    system->update(registry, fixedStep);
                }
            }
        }
        for (auto& [system, status, name] : m_postFixedSystems) {
            if (status) {
                auto&& it = factory.find(utl::StringHash(name));
                ENGINESTATS_SCOPED_TIMER_STR(stats, it->first);
                system->update(registry, fixedStep);
            }
        }
    }


}

void imp::SystemManager::cleanupSystems(entt::registry& registry) const
{
    for (auto&& entry : m_preFixedSystems) {
        entry.system->cleanup(registry);
    }
    for (auto&& entry : m_fixedSystems) {
        entry.system->cleanup(registry);
    }
    for (auto&& entry : m_postFixedSystems) {
        entry.system->cleanup(registry);
    }
    for (auto&& entry : m_systems) {
        entry.system->cleanup(registry);
    }

}

void imp::SystemManager::setSystemStatus(const std::string_view name, bool status)
{

    auto&& it = m_systemMetaMap.find(std::string(name));
    if (it != m_systemMetaMap.end()) {
        Debug::Info("Setting status of {} to {}", name, status);
        getContainer(it->second.type)[it->second.index].status = status;
    }
    else {
        Debug::Error("System not found: {}", name);
    }
}

void imp::SystemManager::onSystemStatusEvent(const SystemStatusEvent& statusEvent)
{
    m_pendingEvents.emplace([this, statusEvent](entt::registry&) {
        setSystemStatus(statusEvent.name, statusEvent.status);
        });
    m_reorderDispatchPending = true;
}

void imp::SystemManager::clear()
{
    m_systems.clear();
    m_fixedSystems.clear();
    m_systemMetaMap.clear();
}
void imp::SystemManager::clearAndCleanup(entt::registry& registry)
{
    for (auto&& entry : m_preFixedSystems) {
        entry.system->cleanup(registry);
    }
    for (auto&& entry : m_fixedSystems) {
        entry.system->cleanup(registry);
    }
    for (auto&& entry : m_postFixedSystems) {
        entry.system->cleanup(registry);
    }
    for (auto&& entry : m_systems) {
        entry.system->cleanup(registry);
    }
    clear();
}

void imp::SystemManager::reload()
{
    size_t variableCount = 0;
    size_t fixedCount = 0;
    size_t preFixedCount = 0;
    size_t postFixedCount = 0;
    for (const auto& [index, type] : m_systemMetaMap | std::views::values) {
        switch (type) {

        case UpdateType::Variable:
            if (index >= variableCount)
                variableCount = index + 1;
            break;
        case UpdateType::Fixed:
            if (index >= fixedCount)
                fixedCount = index + 1;
            break;
        case UpdateType::PreFixed:
            if (index >= m_preFixedSystems.size())
                preFixedCount = index + 1;
            break;
        case UpdateType::PostFixed:
            if (index >= m_postFixedSystems.size())
                postFixedCount = index + 1;
            break;
        default:
            Debug::Error("Invalid system type");
        }

    }
    m_systems.reserve(variableCount);
    m_fixedSystems.reserve(fixedCount);
    m_preFixedSystems.reserve(preFixedCount);
    m_postFixedSystems.reserve(postFixedCount);
    auto&& systemFactory = SystemRegistry::GetSystemFactory();
    for (auto&& [name, info] : m_systemMetaMap) {

        if (auto&& it = systemFactory.find(utl::StringHash(name)); it != systemFactory.end()) {
            Debug::Info("Reloading system: {} at index {}", name, info.index);
            auto&& container = getContainer(info.type);
            if (info.index >= container.size())
                container.resize(info.index + 1);
            container[info.index].system = it->second();
        }
        else {
            Debug::Error("System not registered: {}", name);
        }


    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    m_reorderDispatchPending = true;
}
