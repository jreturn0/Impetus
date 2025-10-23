#include "ComponentEditorRegistry.h"
#include "ComponentInfoRegistry.h"
#include "Components/InputStateComponent.h"
#include "ConfigSystem.h"
#include "ConfigSystem.h"
#include "core/ComputeEffect.h"
#include "CtxRef.h"
#include "CustomComponentWidgets.h"
#include "EditorGuiSystem.h"
#include "EngineStats.h"
#include "events/BasicEvents.h"
#include "Physics/Physics.h"
#include "Renderer.h"
#include "RNG.h"
#include "SystemRegistry.h"
#include "TypeWidgets.h"
#include "window/Window.h"
#include <imgui.h>
#include <ranges>

namespace {
    // Helper function to remove substrings
    void RemoveSubstring(std::string& input, std::string_view toRemove)
    {
        size_t pos = input.find(toRemove);
        while (pos != std::string::npos) {
            input.erase(pos, toRemove.length());
            pos = input.find(toRemove);
        }
    }

    // Function to clean up the type name
    std::string CleanTypeName(const std::string& typeName)
    {
        std::string result = typeName;

        // Remove "Component"
        RemoveSubstring(result, "Component");
        RemoveSubstring(result, "System");

        // Find the position of the last occurrence of "::"
        size_t pos = result.rfind("::");
        if (pos != std::string::npos) {
            // Remove everything before and including "::"
            result = result.substr(pos + 2);
        }
        else {
            // Find and remove "struct " if no "::" was found
            RemoveSubstring(result, "struct ");
            RemoveSubstring(result, "class ");
        }

        return result;
    }

    // Template function to get the cleaned type name
    template<typename T>
    std::string GetCleanTypeName()
    {
        static const std::string cleanName = CleanTypeName(typeid(T).name());
        return cleanName;
    }

    std::string RemoveStructAndNamespace(const std::string& input)
    {
        size_t pos = input.rfind("::");
        if (pos != std::string::npos) {
            return input.substr(pos + 2);
        }
        const std::string structKeyword = "struct ";
        pos = input.find(structKeyword);
        if (pos != std::string::npos) {
            return input.substr(pos + structKeyword.length());
        }

        return input;
    }
    auto cfg_assetsDir = utl::ConfigValueRef("assets.assets_directory", "assets");
}

void EditorGuiSystem::OnSystemsReorderedEvent(SystemsReorderedEvent& event)
{
    m_systemOrder = event;
}

bool EditorGuiSystem::EntityHasComponent(entt::registry& registry, entt::entity& entity, entt::id_type component)
{
    const auto* storage_ptr = registry.storage(component);
    return storage_ptr != nullptr && storage_ptr->contains(entity);
}

void EditorGuiSystem::entityIDWidget(entt::registry& registry, entt::entity& e, bool selectable)
{
    if (e == entt::null) {
        return;
    }
    ImGui::PushID(static_cast<int>(entt::to_integral(e)));

    if (registry.valid(e)) {
        if (selectable) {
            bool isSelected = (e == m_selectedEntity);
            if (ImGui::Selectable(("ID: " + std::to_string(entt::to_integral(e))).c_str(), isSelected)) {
                m_selectedEntity = e;
            }
        }
        else {
            ImGui::Text("ID: %d", entt::to_integral(e));
        }
    }
    else {
        ImGui::Text("Invalid entity");
    }

    ImGui::PopID();
}

void EditorGuiSystem::drawEntityList(entt::registry& registry)
{
    ImGui::Text("Components Filter:");
    ImGui::SameLine();
    if (ImGui::SmallButton("clear")) {
        m_componentFilter.clear();
    }

    ImGui::Indent();
    std::string name;
    for (const auto& [componentTypeId, componentInfo] : imp::ComponentInfoRegistry::GetInfoMap()) {
        bool is_in_list = m_componentFilter.contains(componentTypeId);
        bool active = is_in_list;
        name = CleanTypeName(componentInfo.name);
        ImGui::Checkbox(name.c_str(), &active);

        if (is_in_list && !active) { // remove
            m_componentFilter.erase(componentTypeId);
        }
        else if (!is_in_list && active) { // add
            m_componentFilter.emplace(componentTypeId);
        }
    }
    ImGui::Unindent();
    ImGui::Separator();


    if (ImGui::BeginTabBar("EntityListTabBar")) {
        if (ImGui::BeginTabItem("Filtered")) {

            if (m_componentFilter.empty()) {

                ImGui::Text("Orphans:");
                for (entt::entity e : registry.template storage<entt::entity>()) {
                    if (registry.orphan(e) && registry.valid(e)) {

                        entityIDWidget(registry, e);
                    }
                }
            }
            else {
                entt::basic_runtime_view<entt::basic_sparse_set<entt::entity>> view{};
                for (const auto type : m_componentFilter) {
                    auto* storage_ptr = registry.storage(type);
                    if (storage_ptr != nullptr) {
                        view.iterate(*storage_ptr);
                    }
                }

                // TODO: add support for exclude

                ImGui::Text("%lu Entities Matching:", view.size_hint());

                if (ImGui::BeginChild("entity list")) {
                    for (auto e : view) {
                        entityIDWidget(registry, e);
                    }
                }
                ImGui::EndChild();
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("All")) {
            //ImGui::Text("All:");
            for (entt::entity e : registry.template storage<entt::entity>()) {
                if (registry.valid(e))
                    entityIDWidget(registry, e);
            }

            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

}

void EditorGuiSystem::drawEntityInspector(entt::registry& registry)
{
    ImGui::TextUnformatted("Editing:");
    ImGui::SameLine();
    entityIDWidget(registry, m_selectedEntity, false);
    if (ImGui::Button("New")) {
        m_selectedEntity = registry.create();
    }
    if (registry.valid(m_selectedEntity)) {
        ImGui::SameLine();

        if (ImGui::Button("Clone")) {
            auto old_e = m_selectedEntity;
            m_selectedEntity = registry.create();

            // create a copy of an entity component by component
            for (auto&& curr : registry.storage()) {

                if (auto& storage = curr.second; storage.contains(old_e)) {
                    // TODO: do something with the return value. returns false on failure.
                    if (!storage.contains(m_selectedEntity))
                        storage.push(m_selectedEntity, storage.value(old_e));
                }
            }
        }
        ImGui::SameLine();

        ImGui::Dummy({ 10, 0 }); // space destroy a bit, to not accomponentInfodentally click it
        ImGui::SameLine();

        // red button
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.65f, 0.15f, 0.15f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.3f, 0.3f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.f, 0.2f, 0.2f, 1.f));
        if (ImGui::Button("Destroy")) {
            registry.destroy(m_selectedEntity);
            m_selectedEntity = entt::null;
        }
        ImGui::PopStyleColor(3);
    }
    ImGui::Separator();

    if (registry.valid(m_selectedEntity)) {
        ImGui::PushID(static_cast<int>(entt::to_integral(m_selectedEntity)));
        std::map<entt::id_type, imp::ComponentInfo> has_not;
        for (auto& [componentTypeId, componentInfo] : imp::ComponentInfoRegistry::GetInfoMap()) {
            auto& componentEditorInfo = imp::EditorComponentRegistry::EditorInfo()[componentTypeId];
            if (EntityHasComponent(registry, m_selectedEntity, componentTypeId)) {
                ImGui::PushID(componentTypeId);
                if (ImGui::Button("-")) {
                    componentEditorInfo.destroy(registry, m_selectedEntity);
                    ImGui::PopID();
                    continue; // early out to prevent access to deleted data
                }
                else {
                    ImGui::SameLine();
                }

                if (ImGui::CollapsingHeader(CleanTypeName(componentInfo.name).c_str())) {
                    ImGui::Indent(30.f);
                    ImGui::PushID("Widget");
                    componentEditorInfo.widget(registry, m_selectedEntity);
                    ImGui::PopID();
                    ImGui::Unindent(30.f);
                }
                ImGui::PopID();
            }
            else {
                has_not[componentTypeId] = componentInfo;
            }
        }

        if (!has_not.empty()) {
            if (ImGui::Button("+ Add Component")) {
                ImGui::OpenPopup("Add Component");
            }

            if (ImGui::BeginPopup("Add Component")) {
                ImGui::TextUnformatted("Available:");
                ImGui::Separator();

                for (auto& [componentTypeId, componentInfo] : has_not) {
                    auto& componentEditorInfo = imp::EditorComponentRegistry::EditorInfo()[componentTypeId];
                    ImGui::PushID(componentTypeId);
                    if (ImGui::Selectable(CleanTypeName(componentInfo.name).c_str())) {
                        componentEditorInfo.create(registry, m_selectedEntity);
                    }
                    ImGui::PopID();
                }
                ImGui::EndPopup();
            }
        }
        ImGui::PopID();
    }
}

void EditorGuiSystem::drawMenuBar(entt::registry& registry)
{
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open")) {
                m_showFileSelector = true;
                m_fileHelp.update();
                m_loadOnSelect = true;
            }
            if (ImGui::MenuItem("Save")) {
                registry.ctx().get<CtxRef<entt::dispatcher>>().get().enqueue<QuickSaveSceneEvent>();
            }
            if (ImGui::MenuItem("Save as")) {
                m_showFileSelector = true;
                m_saveOnSelect = true;
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
}

void EditorGuiSystem::drawFileSelector(entt::registry& registry)
{
    if (m_showFileSelector) {
        ImGui::Begin("File Selector", &m_showFileSelector);
        ImGui::Text("Current Path: %s", m_fileHelp.getCurrentPath().c_str());
        ImGui::Separator();

        if (ImGui::Button("Up")) {
            m_fileHelp.goToParent();
        }

        ImGui::Separator();
        // Define colors for directories and files
        constexpr auto directoryColor = ImVec4(0.25f, 0.75f, 1.0f, 1.0f); // Light blue for directories
        constexpr auto fileColor = ImVec4(0.75f, 1.0f, 0.25f, 1.0f); // Light green for files

        // Render directories with custom color
        ImGui::PushStyleColor(ImGuiCol_Text, directoryColor); // Set color for directory
        for (const auto& dir : m_fileHelp.getDirectories()) {
            if (ImGui::Selectable((dir.filename().string() + "/").c_str(), false, ImGuiSelectableFlags_DontClosePopups)) {
                m_fileHelp.goToFolder(dir.string());
                break;
            }
        }
        ImGui::PopStyleColor(); // Reset color

        // Render files with custom color
        ImGui::PushStyleColor(ImGuiCol_Text, fileColor); // Set color for file
        for (const auto& file : m_fileHelp.getFiles()) {
            if (ImGui::Selectable(file.filename().string().c_str(), m_fileHelp.getSelectedFile() && *m_fileHelp.getSelectedFile() == file)) {

                m_fileHelp.setSelectedFile(file.string());
                if (m_loadOnSelect) {
                    registry.ctx().get<CtxRef<entt::dispatcher>>().get().enqueue<LoadSceneEvent>(LoadSceneEvent{ m_fileHelp });
                    m_loadOnSelect = false;
                }
                m_showFileSelector = false; // Close the file selector when a file is selected
                break;
            }
        }
        ImGui::PopStyleColor(); // Reset color

        static char fileName[128] = "";
        if (m_saveOnSelect) {
            ImGui::Separator();
            ImGui::InputText("File Name", fileName, IM_ARRAYSIZE(fileName));

            if (ImGui::Button("Save")) {
                // Ensure the file name is not empty
                if (strlen(fileName) > 0) {
                    registry.ctx().get<CtxRef<entt::dispatcher>>().get().enqueue<SaveSceneAsEvent>(SaveSceneAsEvent{ m_fileHelp ,&fileName[0] });
                    m_saveOnSelect = false;
                    m_showFileSelector = false; // Close the file selector after saving
                }
            }
        }
        ImGui::End();
    }
}

void EditorGuiSystem::drawEngineStats(entt::registry& registry)
{
    auto&& rStats = registry.ctx().get<CtxRef<imp::gfx::Renderer>>().get().getStats();
    auto&& eStats = registry.ctx().get<CtxRef<imp::EngineStats>>().get();
    auto&& timeStatGuiRow = [](std::string_view name, const utl::TimerStats& stats) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("%.*s", static_cast<int>(name.size()), name.data());
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%.2f", stats.totalMs());
        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%.2f", stats.avgMs());
        ImGui::TableSetColumnIndex(3);
        ImGui::Text("%.2f", stats.minMs());
        ImGui::TableSetColumnIndex(4);
        ImGui::Text("%.2f", stats.maxMs());
        };

    static constexpr ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Hideable;

    bool endTableCalled = false;

    if (ImGui::BeginTable("engine Stats", 5, flags)) {
        ImGui::TableSetupColumn("Name    ", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Current ", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Average ", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Min     ", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Max     ", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();


        for (auto&& [hash, stats] : eStats.timeStatsMap) {
            timeStatGuiRow(hash.strView, stats);
        }
        ImGui::EndTable();
        endTableCalled = true;
    }
    if (!endTableCalled) {
        ImGui::Text("EndTable not called for engine Stats");
    }

    endTableCalled = false;

    if (ImGui::BeginTable("renderer Stats", 5, flags)) {
        ImGui::TableSetupColumn("Name    ", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Current ", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Average ", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Min     ", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Max     ", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();

        for (auto&& [hash, stats] : rStats.timeStatsMap) {
            timeStatGuiRow(hash.strView, stats);
        }
        ImGui::EndTable();
        endTableCalled = true;
    }
    if (!endTableCalled) {
        ImGui::Text("EndTable not called for renderer Stats");
    }
}
//
//void EditorGuiSystem::drawGlobalSettings(entt::registry& registry)
//{
//    auto& physics = registry.ctx().get<CtxRef<imp::phys::Physics>>().get();
//    auto& renderer = registry.ctx().get<CtxRef<imp::gfx::Renderer>>().get();
//    auto gravity = physics.getGravity();
//    imp::TypeWidget(gravity, "Gravity");
//    physics.setGravity(gravity);
//
//    auto&& sceneData = renderer.getSceneData();
//    ImGui::SeparatorText("Light");
//    imp::TypeWidget(sceneData.ambientColor, "Ambient Colour");
//
//    glm::vec3 sunlightColor = sceneData.sunlightColor;
//    if (ImGui::DragFloat3("Sunlight Colour", glm::value_ptr(sunlightColor), 0.01f, 0.f, 1.f)) {
//        sceneData.sunlightColor = glm::vec4{ sunlightColor , sceneData.sunlightColor.w };
//    }
//
//
//    imp::TypeWidget(sceneData.sunlightColor.w, "Intensity");
//
//
//    glm::vec3 sunlightDirection = sceneData.sunlightDirection;
//    if (ImGui::DragFloat3("Sunlight Direction", glm::value_ptr(sunlightDirection), 0.01f)) {
//
//        sceneData.sunlightDirection = glm::vec4{ glm::normalize(sunlightDirection) ,sceneData.sunlightDirection.w };
//    }
//
//    imp::TypeWidget(sceneData.sunlightDirection.w, "Distance");
//
//    ImGui::SeparatorText("Compute");
//    auto&& computeEffect = renderer.getCurrentComputeEffect();
//    auto index = renderer.getCurrentComputeIndex();
//    ImGui::SliderInt(computeEffect.getName().data(), &index, 0, renderer.getComputeEffectsSize() - 1);
//    renderer.setCurrentComputeIndex(index);
//    ImGui::Text("Push Constants");
//    ImGui::PushID("Data1");
//    imp::TypeWidget(computeEffect.getPushConstants().data1, "");
//    ImGui::PopID();
//    ImGui::PushID("Data2");
//    imp::TypeWidget(computeEffect.getPushConstants().data2, "");
//    ImGui::PopID();
//    ImGui::PushID("Data3");
//    imp::TypeWidget(computeEffect.getPushConstants().data3, "");
//    ImGui::PopID();
//    ImGui::PushID("Data4");
//    imp::TypeWidget(computeEffect.getPushConstants().data4, "");
//    ImGui::PopID();
//
//}

void EditorGuiSystem::drawSystemsStateEditor(entt::registry& registry)
{
    ImGui::Text("Systems:");

    auto&& systemFactory = imp::SystemRegistry::GetSystemFactory();
    for (const auto& name : systemFactory | std::views::keys) {
        m_nonActiveSystems.insert(std::string(name.strView));
    }
    for (auto& name : m_systemOrder.variable | std::views::keys) {
        m_nonActiveSystems.erase(name);
    }
    for (auto& name : m_systemOrder.fixed | std::views::keys) {
        m_nonActiveSystems.erase(name);
    }
    for (auto& name : m_systemOrder.preFixed | std::views::keys) {
        m_nonActiveSystems.erase(name);
    }
    for (auto& name : m_systemOrder.postFixed | std::views::keys) {
        m_nonActiveSystems.erase(name);
    }
    if (ImGui::Button("+ Add System")) {
        ImGui::OpenPopup("Add System");
    }

    if (ImGui::BeginPopup("Add System")) {
        ImGui::TextUnformatted("Available:");
        ImGui::Separator();

        for (auto& system : m_nonActiveSystems) {
            ImGui::PushID(system.data());
            if (ImGui::Selectable(system.data())) {
                registry.ctx().get<CtxRef<entt::dispatcher>>().get().enqueue<AddSystemEvent>(system, imp::UpdateType::Variable);
            }
            ImGui::PopID();
        }
        ImGui::EndPopup();
    }



    if (ImGui::BeginTable("SystemsOrder", 4)) {  // Only Variable and Fixed columns
        ImGui::TableSetupColumn("Variable");
        ImGui::TableSetupColumn("PreFixed");
        ImGui::TableSetupColumn("Fixed");
        ImGui::TableSetupColumn("PostFixed");
        ImGui::TableHeadersRow();

        size_t maxRows = std::max({ m_systemOrder.variable.size(), m_systemOrder.preFixed.size(), m_systemOrder.fixed.size(), m_systemOrder.postFixed.size() });

        for (size_t row = 0; row < maxRows; ++row) {
            ImGui::TableNextRow();

            // Helper lambda for processing systems
            auto processColumn = [&](auto& systemList, const char* payloadType, imp::UpdateType updateType) {
                ImGui::TableNextColumn();
                if (row < systemList.size()) {
                    auto& [name, status] = systemList[row];
                    std::string displayName = name;
                    ImGui::PushID(displayName.data());
                    if (ImGui::Button("-")) {
                        registry.ctx().get<CtxRef<entt::dispatcher>>().get().enqueue<RemoveSystemEvent>(name);
                    }
                    ImGui::PopID();
                    ImGui::SameLine();
                    if (ImGui::RadioButton(displayName.c_str(), status)) {
                        status = !status;
                        registry.ctx().get<CtxRef<entt::dispatcher>>().get().enqueue<SystemStatusEvent>(name, status);
                    }
                    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                        ImGui::SetDragDropPayload(payloadType, &row, sizeof(size_t)); // Payload type specomponentInfofic to this column
                        ImGui::Text("%s", displayName.c_str());
                        ImGui::EndDragDropSource();
                    }
                    if (ImGui::BeginDragDropTarget()) {
                        auto&& handlePayload = [&](const ImGuiPayload* payload, std::vector<std::pair<std::string, bool>>& systems) {
                            size_t payloadIndex = *static_cast<const size_t*>(payload->Data);
                            auto item = std::move(systems[payloadIndex]);
                            registry.ctx().get<CtxRef<entt::dispatcher>>().get().enqueue<SystemReorderEvent>(item.first, updateType, row);
                            systems.erase(systems.begin() + payloadIndex);
                            systemList.insert(systemList.begin() + row, std::move(item));
                            };

                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("VariableOrder")) {
                            handlePayload(payload, m_systemOrder.variable);
                        }
                        else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PreFixedOrder")) {
                            handlePayload(payload, m_systemOrder.preFixed);
                        }
                        else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FixedOrder")) {
                            handlePayload(payload, m_systemOrder.fixed);
                        }
                        else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PostFixedOrder")) {
                            handlePayload(payload, m_systemOrder.postFixed);
                        }


                        ImGui::EndDragDropTarget();
                    }
                }
                };

            // Process each system type column
            processColumn(m_systemOrder.variable, "VariableOrder", imp::UpdateType::Variable);
            processColumn(m_systemOrder.preFixed, "PreFixedOrder", imp::UpdateType::PreFixed);
            processColumn(m_systemOrder.fixed, "FixedOrder", imp::UpdateType::Fixed);
            processColumn(m_systemOrder.postFixed, "PostFixedOrder", imp::UpdateType::PostFixed);
        }

        ImGui::EndTable();
    }



}

void EditorGuiSystem::drawInputState(entt::registry& registry)
{

    auto input = registry.get<imp::InputStateComponent>(registry.group<imp::InputStateComponent>()[0]);

    ImGui::Text("Mouse Position: %f,%f", input.mouseX, input.mouseY);
    ImGui::Text("Mouse Delta: %f,%f", input.getMouseDelta().x, input.getMouseDelta().y);
    ImGui::Text("Mouse Scroll: %f,%f", input.scrollX, input.scrollY);

    float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
    auto buttonSize = ImGui::CalcTextSize("Button1: 0  ");
    int maxCols = static_cast<int>(glm::floor(window_visible_x2 / buttonSize.x));

    if (ImGui::BeginTable("Mouse Button States", maxCols)) {
        int currentCol = 0;
        for (auto [key, state] : input.mouseButtonStates) {
            ImGui::TableNextColumn();
            ImGui::Text("%s: %i", Input::GetMouseButtonName(key).data(), static_cast<int>(state));
            currentCol++;
            if (currentCol >= maxCols) {
                ImGui::TableNextRow();
                currentCol = 0;
            }
        }
        ImGui::EndTable();
    }

    buttonSize = ImGui::CalcTextSize("RightBracket: 0");
    maxCols = static_cast<int>(glm::floor(window_visible_x2 / buttonSize.x));

    if (ImGui::BeginTable("Key States", maxCols)) {
        int currentCol = 0;
        for (auto [key, state] : input.keyStates) {
            ImGui::TableNextColumn();
            ImGui::Text("%s: %i", Input::GetKeyName(key).data(), static_cast<int>(state));
            currentCol++;
            if (currentCol >= maxCols) {
                ImGui::TableNextRow();
                currentCol = 0;
            }
        }
        ImGui::EndTable();
    }

}

void EditorGuiSystem::drawConfigSettings()
{
    auto&& getCategory = [](std::string_view view) -> std::string_view {
        auto pos = view.find('.');
        if (pos != std::string_view::npos)
            return view.substr(0, pos);
        return { "Global" };
        };
    auto&& getVariable = [](std::string_view view) {
        auto pos = view.find('.');
        if (pos != std::string_view::npos)
            return view.substr(pos + 1);
        return view;
        };
    static auto& cfg = utl::ConfigSystem::instance().getGlobalConfigFile();
    auto& values = cfg.getAllValues();


    std::vector<size_t> indexRange(values.size());
    std::iota(indexRange.begin(), indexRange.end(), 0);
    std::ranges::sort(indexRange, [&](size_t a, size_t b) { return values[a].info->name < values[b].info->name; });

    std::string_view previousCategory{};
    std::string_view currentCategory{};

    for (auto&& [current, initial, info] : indexRange | std::views::transform([&](size_t i)-> auto& { return values[i]; })) {
        currentCategory = getCategory(info->name);
        if (previousCategory != currentCategory)
            ImGui::Text("%.*s :", static_cast<int>(currentCategory.size()), currentCategory.data());
        previousCategory = currentCategory;
        auto name = getVariable(info->name);
        ImGui::PushID(info->name.c_str());
        std::visit([&info, &initial, &name](auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                // Handle string case
                std::string& s = val;
                char buffer[256];
                std::size_t n = std::min(s.size(), sizeof(buffer) - 1);
                std::memcpy(buffer, s.data(), n);
                buffer[n] = '\0';  // null terminate

                if (ImGui::InputText(name.data(), buffer, sizeof(buffer))) {
                    s.assign(buffer);
                }

            }
            else if constexpr (std::is_same_v<T, bool>) {
                ImGui::Checkbox(name.data(), &val);
            }
            else if constexpr (std::is_same_v<T, int64_t>) {
                constexpr int64_t step = 1;
                constexpr int64_t fastStep = 2;
                ImGui::InputScalar(name.data(), ImGuiDataType_S64, &val, &step, &fastStep, "%lld");
            }
            else if constexpr (std::is_same_v<T, double>) {
                ImGui::InputDouble(name.data(), &val);
            }
            else {
                static_assert(false, "Non-exhaustive visitor!");
            }
            // If current != initial, show a button to reset to initial
            // add tooltip to button
            if (val != std::get<T>(initial)) {
                ImGui::SameLine();
                if (ImGui::Button(("Reset##" + info->name).c_str())) {
                    val = std::get<T>(initial);
                }
                ImGui::SetItemTooltip("Reset to initial value");
            }
            }, current);
        ImGui::PopID();
    }

}



void EditorGuiSystem::drawLoadedResources(entt::registry& registry)
{
    auto& renderer = registry.ctx().get<CtxRef<imp::gfx::Renderer>>().get();
    auto& loadedGLTFNames = renderer.getLoadedGLTFNames();
    auto& loadedTextures = renderer.getTextureNames();
    auto& loadedMaterials = renderer.getMaterialNames();



    auto drawTree = [](std::string_view treeName, const std::vector<std::string>& names) {
        constexpr auto baseFlags = ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet;

        if (ImGui::TreeNode(treeName.data())) {
            for (const auto& name : names) {
                ImGui::TreeNodeEx(name.c_str(), baseFlags);
            }
            ImGui::TreePop();
        }
        };

    drawTree("GLTF Models", loadedGLTFNames);
    drawTree("Textures", loadedTextures);
    drawTree("Materials", loadedMaterials);


}



void EditorGuiSystem::drawAll(entt::registry& registry)
{
    ImGui::SetNextWindowSize(ImVec2(550, 400), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_FirstUseEver);
    auto&& renderer = registry.ctx().get<CtxRef<imp::gfx::Renderer>>().get();
    const auto& [triangleCount, drawCallCount, timeStatsMap] = renderer.getStats();
    if (ImGui::Begin("Editor", &m_showWindow, ImGuiWindowFlags_MenuBar)) {
        drawMenuBar(registry);
        drawFileSelector(registry);
        ImGui::Text("fps: %.2f", ImGui::GetIO().Framerate);
        ImGui::SameLine();
        ImGui::Text("triangles: %llu", triangleCount);
        ImGui::SameLine();
        ImGui::Text("draws: %llu", drawCallCount);
        ImGui::SameLine();
        bool culling = renderer.isCulling();
        if (ImGui::Checkbox("Culling", &culling)) {
            renderer.toggleCulling();
        }

        ImGui::Separator();
        if (ImGui::BeginTabBar("EditorTabs")) {
            if (ImGui::BeginTabItem("SceneEntityEditor")) {
                if (ImGui::BeginChild("EntityList", { 200,0 })) {
                    drawEntityList(registry);
                }
                ImGui::EndChild();
                ImGui::SameLine();
                if (ImGui::BeginChild("EntityInspector", { 0,0 })) {
                    drawEntityInspector(registry);
                }
                ImGui::EndChild();
                ImGui::EndTabItem();
            }
            //if (ImGui::BeginTabItem("GlobalSettings")) {
            //    drawGlobalSettings(registry);
            //    ImGui::EndTabItem();
            //}
            if (ImGui::BeginTabItem("LoadedResources")) {
                drawLoadedResources(registry);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("EngineStats")) {
                drawEngineStats(registry);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Systems")) {
                drawSystemsStateEditor(registry);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("InputState")) {
                drawInputState(registry);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Config")) {
                drawConfigSettings();
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
    }
    ImGui::End();
}

namespace {
    auto cfg_meshDir = utl::ConfigValueRef("assets.mesh_directory", "assets/Meshes");
}

void EditorGuiSystem::initialize(entt::registry& registry)
{
    auto& renderer = registry.ctx().get<CtxRef<imp::gfx::Renderer>>().get();
    auto& dispatcher = registry.ctx().get<CtxRef<entt::dispatcher>>().get();
    dispatcher.sink<SystemsReorderedEvent>().connect<&EditorGuiSystem::OnSystemsReorderedEvent>(*this);
    dispatcher.sink<ToggleGuiEvent>().connect<&EditorGuiSystem::OnGuiEvent>(*this);
    renderer.getWindow().setCursorMode(imp::Window::CursorMode::Normal);

    m_actions.emplace_back(
        Input::Key::F8, Input::State::JustPressed, [&registry]() {
            registry.ctx().get<CtxRef<entt::dispatcher>>().get().enqueue<ShutdownEvent>();
        });

    m_actions.emplace_back(
        Input::Key::F4, Input::State::JustPressed, [&renderer]() {
            renderer.getWindow().setCursorMode(renderer.getWindow().getCursorMode() == imp::Window::CursorMode::Disabled ? imp::Window::CursorMode::Normal : imp::Window::CursorMode::Disabled);
        });
    m_actions.emplace_back(
        Input::Key::F3, Input::State::JustPressed, [&dispatcher]() {
            dispatcher.enqueue<ToggleGuiEvent>();
        });
    m_actions.emplace_back(
        Input::Key::F6, Input::State::JustPressed, [&renderer]() {
            renderer.reloadAllShaders();
        });
    static bool loadedMeshes = false;
    if (!loadedMeshes) {
        loadedMeshes = true;

        FSNavigator fd;
        fd.goToPath(fd.getCurrentPath() / cfg_meshDir.get().c_str());

        for (auto& file : fd.getFiles()) {
            auto start = std::chrono::high_resolution_clock::now();
            renderer.loadGLTF(fd.getFilePath(file));
            auto end = std::chrono::high_resolution_clock::now();
            Debug::Info("File: {} Loaded in: {}ms", file.filename().string(), std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
        }
    }
    m_fileHelp.goToPath(m_fileHelp.getCurrentPath() / cfg_assetsDir.get().c_str());

}

void EditorGuiSystem::update(entt::registry& registry, const float deltaTime)
{
    auto&& group = registry.group_if_exists<imp::InputStateComponent>();
    //auto& renderer = registry.ctx().get<CtxRef<Imp::Render::Renderer>>().get();
    if (!group.empty()) {
        auto&& input = registry.try_get<imp::InputStateComponent>(group[0]);
        for (auto&& [key, state, action] : m_actions) {
            if (input->keyStates[key] == state) {
                action();
            }
        }
    }
    if (m_gui) {
        drawAll(registry);
    }
}

void EditorGuiSystem::cleanup(entt::registry& registry) { System::cleanup(registry); }
