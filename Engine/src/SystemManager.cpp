#include "SystemManager.h"
#include "SystemRegistry.h"
#include <iostream>
#include "Clock.h"
#include <Debug.h>
#include <ranges>

#include "CtxRef.h"
#include "EngineStats.h"


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
        return systems;
    case Fixed:
        return fixedSystems;
    case PreFixed:
        return preFixedSystems;
    case PostFixed:
        return postFixedSystems;
    default:
        Debug::Throw("Invalid System UpdateType: {}", static_cast<size_t>(type));
        return systems;
    }
}

void imp::SystemManager::OnSystemReorderEvent(SystemReorderEvent& event)
{
    if (!systemMetaMap.contains(event.name)) {
        Debug::Error("System not found: {}", event.name);
        return;
    }
    systemEvents.emplace([this, event](entt::registry&) {
        auto&& it = systemMetaMap.find(event.name);
        if (it == systemMetaMap.end()) {
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
                systemMetaMap[container[i].name].index = i;
            }
            for (auto i = 0; i < newContainer.size(); i++) {
                systemMetaMap[newContainer[i].name].index = i;
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
            systemMetaMap[container[i].name].index = i;
        }
        });



}

void imp::SystemManager::OnSystemAddEvent(AddSystemEvent& event)
{
    systemEvents.emplace([this, event](entt::registry& registry) {
        addSystem(event.name, false, event.type);
        getContainer(event.type)[systemMetaMap[event.name].index].system->initialize(registry);
        Debug::Info("System added: {}", event.name);
        });
    reorderDispatchPending = true;
}

void imp::SystemManager::OnSystemRemoveEvent(RemoveSystemEvent& event)
{
    auto it = systemMetaMap.find(event.name);
    if (it != systemMetaMap.end()) {
        auto&& [name, meta] = *it;
        auto&& container = getContainer(meta.type);
        container.erase(container.begin() + meta.index);
        systemMetaMap.erase(it);
        for (auto i = 0; i < container.size(); i++) {
            systemMetaMap[container[i].name].index = i;
        }
        reorderDispatchPending = true;
    }
    else {
        Debug::Error("System not found: {}", event.name);
    }
}


void imp::SystemManager::addSystem(const std::string& name, bool enabled, UpdateType type)
{
    auto&& systemFactory = SystemRegistry::GetSystemFactory();
    if (const auto it = systemFactory.find(name); it != systemFactory.end()) {
        Entry entry = { it->second(), enabled ,it->first };
        auto&& container = getContainer(type);
        container.push_back(std::move(entry));
        systemMetaMap[it->first] = { container.size() - 1, type };

        reorderDispatchPending = true;
    }
    else {
        Debug::Error("System not registered: {}", name);
    }
}

void imp::SystemManager::initializeSystems(entt::registry& registry)
{
    try {
        for (auto&& entry : preFixedSystems) {
            entry.system->initialize(registry);
        }
        for (auto&& entry : fixedSystems) {
            entry.system->initialize(registry);
        }
        for (auto&& entry : postFixedSystems) {
            entry.system->initialize(registry);
        }
        for (auto&& entry : systems) {
            entry.system->initialize(registry);
        }
    }
    catch (const std::exception& e) {
        Debug::Exception("initializing systems: {}", e.what());
    }
    registry.ctx().get<CtxRef<entt::dispatcher>>().get().sink<SystemReorderEvent>().connect<&SystemManager::OnSystemReorderEvent>(*this);
    registry.ctx().get<CtxRef<entt::dispatcher>>().get().sink<AddSystemEvent>().connect<&SystemManager::OnSystemAddEvent>(*this);
    registry.ctx().get<CtxRef<entt::dispatcher>>().get().sink<RemoveSystemEvent>().connect<&SystemManager::OnSystemRemoveEvent>(*this);
    reorderDispatchPending = true;
}

void imp::SystemManager::updateSystems(entt::registry& registry, Clock& time, EngineStats& stats)
{

    const double deltaTime = time.getDelta();
    while (!systemEvents.empty()) {
        systemEvents.front()(registry);
        systemEvents.pop();
    }
    if (reorderDispatchPending) {
        SystemsReorderedEvent reorderedEvent;

        reorderedEvent.variable.reserve(systems.size());
        for (auto&& [system, status, name] : systems) {
            reorderedEvent.variable.emplace_back(name, status);
        }
        reorderedEvent.preFixed.reserve(systems.size());
        for (auto&& [system, status, name] : preFixedSystems) {
            reorderedEvent.preFixed.emplace_back(name, status);
        }
        reorderedEvent.fixed.reserve(systems.size());
        for (auto&& [system, status, name] : fixedSystems) {
            reorderedEvent.fixed.emplace_back(name, status);
        }
        reorderedEvent.postFixed.reserve(systems.size());
        for (auto&& [system, status, name] : postFixedSystems) {
            reorderedEvent.postFixed.emplace_back(name, status);
        }

        registry.ctx().get<CtxRef<entt::dispatcher>>().get().enqueue<SystemsReorderedEvent>(std::move(reorderedEvent));
        reorderDispatchPending = false;
    }

    for (auto& [system, status, name] : systems) {
        if (status) {
            ENGINESTATS_SCOPED_TIMER_STR(stats, name);
            system->update(registry, static_cast<float>(deltaTime));
        }
    }
    if (time.isFixed()) {
        const float fixedStep = static_cast<float>(time.getFixedStep());
        //Debug::Info("FixedStep {}", fixedStep);

        for (auto& [system, status, name] : preFixedSystems) {
            if (status) {
                ENGINESTATS_SCOPED_TIMER_STR(stats, name);
                system->update(registry, fixedStep);
            }
        }
        while (time.fixedUpdate()) {
            for (auto& [system, status, name] : fixedSystems) {
                if (status) {
                    ENGINESTATS_SCOPED_TIMER_STR(stats, name);
                    system->update(registry, fixedStep);
                }
            }
        }
        for (auto& [system, status, name] : postFixedSystems) {
            if (status) {
                ENGINESTATS_SCOPED_TIMER_STR(stats, name);
                system->update(registry, fixedStep);
            }
        }
    }


}

void imp::SystemManager::cleanupSystems(entt::registry& registry) const
{
    for (auto&& entry : preFixedSystems) {
        entry.system->cleanup(registry);
    }
    for (auto&& entry : fixedSystems) {
        entry.system->cleanup(registry);
    }
    for (auto&& entry : postFixedSystems) {
        entry.system->cleanup(registry);
    }
    for (auto&& entry : systems) {
        entry.system->cleanup(registry);
    }

}

void imp::SystemManager::setSystemStatus(const std::string& name, bool status)
{
    auto it = systemMetaMap.find(name);
    if (it != systemMetaMap.end()) {
        Debug::Info("Setting status of {} to {}", name, status);
        getContainer(it->second.type)[it->second.index].status = status;
    }
    else {
        Debug::Error("System not found: {}", name);
    }
}

void imp::SystemManager::onSystemStatusEvent(const SystemStatusEvent& statusEvent)
{
    systemEvents.emplace([this, statusEvent](entt::registry&) {
        setSystemStatus(statusEvent.name, statusEvent.status);
        });
    reorderDispatchPending = true;
}

void imp::SystemManager::clear()
{
    systems.clear();
    fixedSystems.clear();
    systemMetaMap.clear();
}
void imp::SystemManager::clearAndCleanup(entt::registry& registry)
{
    for (auto&& entry : preFixedSystems) {
        entry.system->cleanup(registry);
    }
    for (auto&& entry : fixedSystems) {
        entry.system->cleanup(registry);
    }
    for (auto&& entry : postFixedSystems) {
        entry.system->cleanup(registry);
    }
    for (auto&& entry : systems) {
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
    for (const auto& [index, type] : systemMetaMap | std::views::values) {
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
            if (index >= preFixedSystems.size())
                preFixedCount = index + 1;
            break;
        case UpdateType::PostFixed:
            if (index >= postFixedSystems.size())
                postFixedCount = index + 1;
            break;
        default:
            Debug::Error("Invalid system type");
        }

    }
    systems.reserve(variableCount);
    fixedSystems.reserve(fixedCount);
    preFixedSystems.reserve(preFixedCount);
    postFixedSystems.reserve(postFixedCount);
    auto&& systemFactory = SystemRegistry::GetSystemFactory();
    for (auto&& [name, info] : systemMetaMap) {

        if (auto&& it = systemFactory.find(name); it != systemFactory.end()) {
            Debug::Info("Reloading system: {} at index {}", name, info.index);
            if (name == "Imp::CameraSystem") {
                //Debug::Info("CameraSystem");
            }
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
    reorderDispatchPending = true;
}
