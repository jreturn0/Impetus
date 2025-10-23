#pragma once
#include "Events/SystemEvents.h"
#include "FSNavigator.h"
#include "InputCodes.h"
#include "System.h"
#include <entt/entt.hpp>
#include <functional>
#include <set>
#include <string>
#include <typeinfo>
#include <vector>

class EditorGuiSystem final : public System
{
public:
    struct InputAction
    {
        Input::Key key;
        Input::State state;
        std::function<void()> action;
        inline bool operator==(const InputAction& other) const noexcept { return key == other.key && state == other.state; }
        inline void operator()() const { if (action) action(); }
    };

    void initialize(entt::registry& registry) override;
    void update(entt::registry& registry, const float deltaTime) override;
    void cleanup(entt::registry& registry) override;

    inline void OnGuiEvent() { m_gui = !m_gui; }
private:
    std::vector<InputAction> m_actions{};
    bool m_showWindow{true};
    bool m_dropTarget{false};
    entt::entity m_selectedEntity{ entt::null };
    std::set<entt::id_type> m_componentFilter{};
    bool m_showFileSelector{false};
    bool m_saveOnSelect{false};
    bool m_loadOnSelect{false};
    bool m_gui{true};
    std::set<std::string> m_nonActiveSystems{};
    FSNavigator m_fileHelp{};
    SystemsReorderedEvent m_systemOrder{};



    void OnSystemsReorderedEvent(SystemsReorderedEvent& event);

    static bool EntityHasComponent(entt::registry& registry, entt::entity& entity, entt::id_type component);

    void entityIDWidget(entt::registry& registry, entt::entity& e, bool selectable = true);
    void drawEntityList(entt::registry& registry);
    void drawEntityInspector(entt::registry& registry);
    void drawMenuBar(entt::registry& registry);
    void drawFileSelector(entt::registry& registry);
    void drawEngineStats(entt::registry& registry);
    //void drawGlobalSettings(entt::registry& registry);
    void drawSystemsStateEditor(entt::registry& registry);
    void drawInputState(entt::registry& registry);
    void drawConfigSettings();
    void drawLoadedResources(entt::registry& registry);
    void drawAll(entt::registry& registry);

};

namespace imp {




}
