#include "EditorGuiSystem.h"

#include <imgui.h>
#include <ranges>

#include "BasicEvents.h"
#include "ComponentInfoRegistry.h"
#include "CtxRef.h"
#include "EngineStats.h"
#include "Renderer.h"
#include "RNG.h"
#include "SystemRegistry.h"
#include "Components/InputStateComponent.h"
#include "core/ComputeEffect.h"
#include "Physics/Physics.h"
#include "window/Window.h"
#include "ConfigSystem.h"

void EditorGuiSystem::OnSystemsReorderedEvent(SystemsReorderedEvent& event)
{
    systemOrder = event;
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
        // Display entity ID as selectable if selectedEntity is provided
        if (selectable) {
            bool isSelected = (e == selectedEntity);
            if (ImGui::Selectable(("ID: " + std::to_string(entt::to_integral(e))).c_str(), isSelected)) {
                selectedEntity = e;
                //  Debug::LogInfo("Selected entt::entity: {}", entt::to_integral(selectedEntity));
            }
        }
        else {
            // Display entity ID as text if no selection is required
            ImGui::Text("ID: %d", entt::to_integral(e));
        }
    }
    else {
        ImGui::Text("Invalid entity");
    }

    ImGui::PopID();
}
std::string RemoveStructAndNamespace(const std::string& input)
{
    // Find the position of the last occurrence of "::"
    size_t pos = input.rfind("::");

    if (pos != std::string::npos) {
        // Remove everything before and including "::"
        return input.substr(pos + 2);
    }

    // Find and remove "struct " if no "::" was found
    const std::string structKeyword = "struct ";

    pos = input.find(structKeyword);

    if (pos != std::string::npos) {
        return input.substr(pos + structKeyword.length());
    }

    // Return the original string if neither "struct " nor "::" is found
    return input;
}
void EditorGuiSystem::drawEntityList(entt::registry& registry)
{
    ImGui::Text("Components Filter:");
    ImGui::SameLine();
    if (ImGui::SmallButton("clear")) {
        componentFilter.clear();
    }

    ImGui::Indent();
    std::string name;
    for (const auto& [component_type_id, ci] : imp::ComponentInfoRegistry::GetInfoMap()) {
        bool is_in_list = componentFilter.contains(component_type_id);
        bool active = is_in_list;
        name = CleanTypeName(ci.name);
        ImGui::Checkbox(name.c_str(), &active);

        if (is_in_list && !active) { // remove
            componentFilter.erase(component_type_id);
        }
        else if (!is_in_list && active) { // add
            componentFilter.emplace(component_type_id);
        }
    }
    ImGui::Unindent();
    ImGui::Separator();


    if (ImGui::BeginTabBar("EntityListTabBar")) {
        if (ImGui::BeginTabItem("Filtered")) {

            if (componentFilter.empty()) {

                ImGui::Text("Orphans:");
                for (entt::entity e : registry.template storage<entt::entity>()) {
                    if (registry.orphan(e) && registry.valid(e)) {

                        entityIDWidget(registry, e);
                    }
                }
            }
            else {
                entt::basic_runtime_view<entt::basic_sparse_set<entt::entity>> view{};
                for (const auto type : componentFilter) {
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
    entityIDWidget(registry, selectedEntity, false);
    if (ImGui::Button("New")) {
        selectedEntity = registry.create();
    }
    if (registry.valid(selectedEntity)) {
        ImGui::SameLine();

        if (ImGui::Button("Clone")) {
            auto old_e = selectedEntity;
            selectedEntity = registry.create();

            // create a copy of an entity component by component
            for (auto&& curr : registry.storage()) {

                if (auto& storage = curr.second; storage.contains(old_e)) {
                    // TODO: do something with the return value. returns false on failure.
                    if (!storage.contains(selectedEntity))
                        storage.push(selectedEntity, storage.value(old_e));
                }
            }
        }
        ImGui::SameLine();

        ImGui::Dummy({ 10, 0 }); // space destroy a bit, to not accidentally click it
        ImGui::SameLine();

        // red button
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.65f, 0.15f, 0.15f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.3f, 0.3f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.f, 0.2f, 0.2f, 1.f));
        if (ImGui::Button("Destroy")) {
            registry.destroy(selectedEntity);
            selectedEntity = entt::null;
        }
        ImGui::PopStyleColor(3);
    }
    ImGui::Separator();



    if (registry.valid(selectedEntity)) {
        ImGui::PushID(static_cast<int>(entt::to_integral(selectedEntity)));
        std::map<entt::id_type, imp::ComponentInfo> has_not;
        for (auto& [component_type_id, ci] : imp::ComponentInfoRegistry::GetInfoMap()) {
            if (EntityHasComponent(registry, selectedEntity, component_type_id)) {
                ImGui::PushID(component_type_id);
                if (ImGui::Button("-")) {
                    ci.destroy(registry, selectedEntity);
                    ImGui::PopID();
                    continue; // early out to prevent access to deleted data
                }
                else {
                    ImGui::SameLine();
                }

                if (ImGui::CollapsingHeader(CleanTypeName(ci.name).c_str())) {
                    ImGui::Indent(30.f);
                    ImGui::PushID("Widget");
                    ci.widget(registry, selectedEntity);
                    ImGui::PopID();
                    ImGui::Unindent(30.f);
                }
                ImGui::PopID();
            }
            else {
                has_not[component_type_id] = ci;
            }
        }

        if (!has_not.empty()) {
            if (ImGui::Button("+ Add Component")) {
                ImGui::OpenPopup("Add Component");
            }

            if (ImGui::BeginPopup("Add Component")) {
                ImGui::TextUnformatted("Available:");
                ImGui::Separator();

                for (auto& [component_type_id, ci] : has_not) {
                    ImGui::PushID(component_type_id);
                    if (ImGui::Selectable(CleanTypeName(ci.name).c_str())) {
                        ci.create(registry, selectedEntity);
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
                showFileSelector = true;
                fileHelp.update();
                loadOnSelect = true;
            }
            if (ImGui::MenuItem("Save")) {
                if (fileHelp.getSelectedFile().has_value()) {
                    registry.ctx().get<CtxRef<entt::dispatcher>>().get().enqueue<SaveSceneAsEvent>(fileHelp);
                    // Use the selected file path
                    //std::string filePath = *selectedFile;
                    //save(selectedFile.value());
                    // Handle file opening logic here
                }
                else {
                    showFileSelector = true;
                    saveOnSelect = true;
                }
            }
            if (ImGui::MenuItem("Save as")) {
                showFileSelector = true;
                saveOnSelect = true;
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
}

void EditorGuiSystem::drawFileSelector(entt::registry& registry)
{
    if (showFileSelector) {
        ImGui::Begin("File Selector", &showFileSelector);

        ImGui::Text("Current Path: %s", fileHelp.getCurrentPath().c_str());
        ImGui::Separator();

        if (ImGui::Button("Up")) {
            fileHelp.goToParent();
        }

        ImGui::Separator();
        // Define colors for directories and files
        constexpr auto directoryColor = ImVec4(0.25f, 0.75f, 1.0f, 1.0f); // Light blue for directories
        constexpr auto fileColor = ImVec4(0.75f, 1.0f, 0.25f, 1.0f); // Light green for files

        // Render directories with custom color
        ImGui::PushStyleColor(ImGuiCol_Text, directoryColor); // Set color for directory
        for (const auto& dir : fileHelp.getDirectories()) {
            if (ImGui::Selectable((dir.filename().string() + "/").c_str(), false, ImGuiSelectableFlags_DontClosePopups)) {
                fileHelp.goToFolder(dir.string());
                break;
            }
        }
        ImGui::PopStyleColor(); // Reset color

        // Render files with custom color
        ImGui::PushStyleColor(ImGuiCol_Text, fileColor); // Set color for file
        for (const auto& file : fileHelp.getFiles()) {
            if (ImGui::Selectable(file.filename().string().c_str(), fileHelp.getSelectedFile() && *fileHelp.getSelectedFile() == file)) {

                fileHelp.setSelectedFile(file.string());
                if (loadOnSelect) {
                    registry.ctx().get<CtxRef<entt::dispatcher>>().get().enqueue<LoadSceneEvent>(LoadSceneEvent{ fileHelp });
                    loadOnSelect = false;
                }
                showFileSelector = false; // Close the file selector when a file is selected
                break;
            }
        }
        ImGui::PopStyleColor(); // Reset color

        static char fileName[128] = "";
        if (saveOnSelect) {
            ImGui::Separator();
            ImGui::InputText("File Name", fileName, IM_ARRAYSIZE(fileName));

            if (ImGui::Button("Save")) {
                // Ensure the file name is not empty
                if (strlen(fileName) > 0) {
                    // Combine the current path and file name to get the full path
                    //fileHelp.setSelectedFile(fileName);
                    registry.ctx().get<CtxRef<entt::dispatcher>>().get().enqueue<SaveSceneAsEvent>(SaveSceneAsEvent{ fileHelp ,&fileName[0] });
                    // fileHelp.setSelectedFile(fileName);
                    saveOnSelect = false;
                    showFileSelector = false; // Close the file selector after saving
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

    //static size_t rollingIndex = 0;
    //static std::unordered_map<std::string_view, std::array<long long, 64>> rollingStats;

    auto&& timeStatGuiRow = [](std::string_view name, const utl::TimerStats& stats) {


        //auto&& rolling = rollingStats[name];
        //const long long accumulatedFrameTime = std::accumulate(rolling.begin(), rolling.end(), 0ll);
        //const float averageFrameTime = static_cast<float>(accumulatedFrameTime) / rolling.size(); // in the same units as frameTime

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("%s", name.data());
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%.2f", stats.totalMs());
        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%.2f", stats.avgMs());
        ImGui::TableSetColumnIndex(3);
        ImGui::Text("%.2f", stats.minMs());
        ImGui::TableSetColumnIndex(4);
        ImGui::Text("%.2f", stats.maxMs());
        };

    static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Hideable;

    bool endTableCalled = false;

    if (ImGui::BeginTable("engine Stats", 5, flags)) {
        ImGui::TableSetupColumn("Name    ", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Current ", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Average ", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Min     ", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Max     ", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();

        //timeStatGuiRow("Frame", eStats.frameTime);
        //timeStatGuiRow("Event Dispatch", eStats.eventDispatchTime);
        //timeStatGuiRow("Update", eStats.updateTime);
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

        for (auto&& [hash, stats] : eStats.timeStatsMap) {
            timeStatGuiRow(hash.strView, stats);
        }
        ImGui::EndTable();
        endTableCalled = true;
    }
    if (!endTableCalled) {
        ImGui::Text("EndTable not called for renderer Stats");
    }
    //rollingIndex = (rollingIndex + 1) % 64;
}

void EditorGuiSystem::drawGlobalSettings(entt::registry& registry)
{
    auto& physics = registry.ctx().get<CtxRef<imp::Phys::Physics>>().get();
    auto& renderer = registry.ctx().get<CtxRef<imp::gfx::Renderer>>().get();
    auto gravity = physics.getGravity();
    imp::TypeWidget(gravity, "Gravity");
    physics.setGravity(gravity);

    auto&& sceneData = renderer.getSceneData();
    ImGui::SeparatorText("Light");
    imp::TypeWidget(sceneData.ambientColor, "Ambient Colour");
    imp::TypeWidget(sceneData.sunlightColor, "Sunlight Colour");
    glm::vec3 sunlightDirection = sceneData.sunlightDirection;
    if (ImGui::DragFloat3("Sunlight Direction", glm::value_ptr(sunlightDirection), 0.01f)) {

        sceneData.sunlightDirection = glm::vec4{ glm::normalize(sunlightDirection) ,sceneData.sunlightDirection.w };
    }
    ImGui::DragFloat("Power", &sceneData.sunlightDirection.w, 0.01f);
    ImGui::SeparatorText("Compute");
    auto&& computeEffect = renderer.getCurrentComputeEffect();
    auto index = renderer.getCurrentComputeIndex();
    ImGui::SliderInt(computeEffect.getName().data(), &index, 0, renderer.getComputeEffectsSize() - 1);
    renderer.setCurrentComputeIndex(index);
    ImGui::Text("Push Constants");
    ImGui::PushID("Data1");
    imp::TypeWidget(computeEffect.getPushConstants().data1, "");
    ImGui::PopID();
    ImGui::PushID("Data2");
    imp::TypeWidget(computeEffect.getPushConstants().data2, "");
    ImGui::PopID();
    ImGui::PushID("Data3");
    imp::TypeWidget(computeEffect.getPushConstants().data3, "");
    ImGui::PopID();
    ImGui::PushID("Data4");
    imp::TypeWidget(computeEffect.getPushConstants().data4, "");
    ImGui::PopID();

}

void EditorGuiSystem::drawSystemsStateEditor(entt::registry& registry)
{
    ImGui::Text("Systems:");

    auto&& systemFactory = imp::SystemRegistry::GetSystemFactory();
    for (const auto& name : systemFactory | std::views::keys) {
        nonActiveSystems.insert(name);
    }
    for (auto& name : systemOrder.variable | std::views::keys) {
        nonActiveSystems.erase(name);
    }
    for (auto& name : systemOrder.fixed | std::views::keys) {
        nonActiveSystems.erase(name);
    }	for (auto& name : systemOrder.preFixed | std::views::keys) {
        nonActiveSystems.erase(name);
    }	for (auto& name : systemOrder.postFixed | std::views::keys) {
        nonActiveSystems.erase(name);
    }
    if (ImGui::Button("+ Add System")) {
        ImGui::OpenPopup("Add System");
    }

    if (ImGui::BeginPopup("Add System")) {
        ImGui::TextUnformatted("Available:");
        ImGui::Separator();

        for (auto& system : nonActiveSystems) {
            ImGui::PushID(system.c_str());
            if (ImGui::Selectable(CleanTypeName(system).c_str())) {
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

        size_t maxRows = std::max({ systemOrder.variable.size(), systemOrder.preFixed.size(), systemOrder.fixed.size(), systemOrder.postFixed.size() });

        for (size_t row = 0; row < maxRows; ++row) {
            ImGui::TableNextRow();

            // Helper lambda for processing systems
            auto processColumn = [&](auto& systemList, const char* payloadType, imp::UpdateType updateType) {
                ImGui::TableNextColumn();
                if (row < systemList.size()) {
                    auto& [name, status] = systemList[row];
                    auto displayName = CleanTypeName(name);
                    ImGui::PushID(displayName.c_str());
                    if (ImGui::Button("-")) {
                        registry.ctx().get<CtxRef<entt::dispatcher>>().get().enqueue<RemoveSystemEvent>(name);
                    }
                    ImGui::PopID();
                    ImGui::SameLine();
                    if (ImGui::RadioButton(displayName.data(), status)) {
                        status = !status;
                        registry.ctx().get<CtxRef<entt::dispatcher>>().get().enqueue<SystemStatusEvent>(name, status);
                    }
                    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                        ImGui::SetDragDropPayload(payloadType, &row, sizeof(size_t)); // Payload type specific to this column
                        ImGui::Text("%s", displayName.c_str());
                        ImGui::EndDragDropSource();
                    }
                    if (ImGui::BeginDragDropTarget()) {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("VariableOrder")) {
                            size_t payloadIndex = *static_cast<const size_t*>(payload->Data);
                            auto item = std::move(systemOrder.variable[payloadIndex]);
                            registry.ctx().get<CtxRef<entt::dispatcher>>().get().enqueue<SystemReorderEvent>(item.first, updateType, row);
                            systemOrder.variable.erase(systemOrder.variable.begin() + payloadIndex);
                            systemList.insert(systemList.begin() + row, std::move(item));
                        }
                        else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PreFixedOrder")) {
                            size_t payloadIndex = *static_cast<const size_t*>(payload->Data);
                            auto item = std::move(systemOrder.preFixed[payloadIndex]);
                            registry.ctx().get<CtxRef<entt::dispatcher>>().get().enqueue<SystemReorderEvent>(item.first, updateType, row);
                            systemOrder.preFixed.erase(systemOrder.preFixed.begin() + payloadIndex);
                            systemList.insert(systemList.begin() + row, std::move(item));
                        }
                        else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FixedOrder")) {
                            size_t payloadIndex = *static_cast<const size_t*>(payload->Data);
                            auto item = std::move(systemOrder.fixed[payloadIndex]);
                            registry.ctx().get<CtxRef<entt::dispatcher>>().get().enqueue<SystemReorderEvent>(item.first, updateType, row);
                            systemOrder.fixed.erase(systemOrder.fixed.begin() + payloadIndex);
                            systemList.insert(systemList.begin() + row, std::move(item));
                        }
                        else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PostFixedOrder")) {
                            size_t payloadIndex = *static_cast<const size_t*>(payload->Data);
                            auto item = std::move(systemOrder.postFixed[payloadIndex]);
                            registry.ctx().get<CtxRef<entt::dispatcher>>().get().enqueue<SystemReorderEvent>(item.first, updateType, row);
                            systemOrder.postFixed.erase(systemOrder.postFixed.begin() + payloadIndex);
                            systemList.insert(systemList.begin() + row, std::move(item));
                        }
                        ImGui::EndDragDropTarget();
                    }
                }
                };

            // Process each system type column
            processColumn(systemOrder.variable, "VariableOrder", imp::UpdateType::Variable);
            processColumn(systemOrder.preFixed, "PreFixedOrder", imp::UpdateType::PreFixed);
            processColumn(systemOrder.fixed, "FixedOrder", imp::UpdateType::Fixed);
            processColumn(systemOrder.postFixed, "PostFixedOrder", imp::UpdateType::PostFixed);
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

    static auto& cfg = utl::ConfigSystem::instance().getGlobalConfigFile();
    auto& values = cfg.getAllValues();

    for (auto&& [current, initial, info] : values) {
        std::visit([&info, &initial](auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                // Handle string case
                std::string& s = val;
                char buffer[256];
                std::size_t n = std::min(s.size(), sizeof(buffer) - 1);
                std::memcpy(buffer, s.data(), n);
                buffer[n] = '\0';  // null terminate
                if (ImGui::InputText(info->name.c_str(), buffer, sizeof(buffer))) {
                    s.assign(buffer);
                }
                // If current != initial, show a button to reset to initial
                // add tooltip to button
                if (s != std::get<std::string>(initial)) {
                    ImGui::SameLine();
                    if (ImGui::Button(("Reset##" + info->name).c_str())) {
                        s = std::get<std::string>(initial);
                    }
                    ImGui::SetItemTooltip("Reset to initial value");
                }
            }
            else if constexpr (std::is_same_v<T, bool>) {
                ImGui::Checkbox(info->name.c_str(), &val);
            }
            else if constexpr (std::is_same_v<T, int64_t>) {
                constexpr int64_t step = 1;
                constexpr int64_t fastStep = 2;
                ImGui::InputScalar(info->name.c_str(), ImGuiDataType_S64, &val, &step, &fastStep, "%lld");
            }
            else if constexpr (std::is_same_v<T, double>) {
                ImGui::InputDouble(info->name.c_str(), &val);
            }
            else {
                static_assert(false, "Non-exhaustive visitor!");
            }
            }, current);
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


void DrawBlackJack()
{
    static int playerMoney = 1000;
    static int playerScore = 0;
    static int dealerScore = 0;
    struct Card
    {
        enum class Rank
        {
            Ace = 1,
            Two = 2,
            Three = 3,
            Four = 4,
            Five = 5,
            Six = 6,
            Seven = 7,
            Eight = 8,
            Nine = 9,
            Ten = 10,
            Jack = 10,
            Queen = 10,
            King = 10
        } rank;
        enum class Suits
        {
            Hearts,
            Diamonds,
            Clubs,
            Spades
        } suit;
    };
    size_t deckPos = 0;
    //6 decks
    std::array<Card, 312> deck;
    for (int i = 0; i < 4; i++) {
        const auto suit = static_cast<Card::Suits>(i);
        for (int j = 0; j < 13; j++) {
            deck[deckPos++] = { static_cast<Card::Rank>(j),  suit };
        }
    }
    utl::RNG::Shuffle(deck);
    deckPos = 0;



}

void EditorGuiSystem::drawAll(entt::registry& registry)
{
    ImGui::SetNextWindowSize(ImVec2(550, 400), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_FirstUseEver);
    auto&& renderer = registry.ctx().get<CtxRef<imp::gfx::Renderer>>().get();
    const auto& [triangleCount, drawCallCount, timeStatsMap] = renderer.getStats();
    if (ImGui::Begin("Editor", &showWindow, ImGuiWindowFlags_MenuBar)) {
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
            if (ImGui::BeginTabItem("GlobalSettings")) {
                drawGlobalSettings(registry);
                ImGui::EndTabItem();
            }
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

void EditorGuiSystem::initialize(entt::registry& registry)
{
    auto& renderer = registry.ctx().get<CtxRef<imp::gfx::Renderer>>().get();
    auto& dispatcher = registry.ctx().get<CtxRef<entt::dispatcher>>().get();
    dispatcher.sink<SystemsReorderedEvent>().connect<&EditorGuiSystem::OnSystemsReorderedEvent>(*this);
    dispatcher.sink<ToggleGuiEvent>().connect<&EditorGuiSystem::OnGuiEvent>(*this);
    renderer.getWindow().setCursorMode(imp::Window::CursorMode::Normal);

    actions.emplace_back(
        Input::Key::F8, Input::State::JustPressed, [&registry]() {
            registry.ctx().get<CtxRef<entt::dispatcher>>().get().enqueue<ShutdownEvent>();
        });

    actions.emplace_back(
        Input::Key::F4, Input::State::JustPressed, [&renderer]() {

            renderer.getWindow().setCursorMode(renderer.getWindow().getCursorMode() == imp::Window::CursorMode::Disabled ? imp::Window::CursorMode::Normal : imp::Window::CursorMode::Disabled);
        });
    actions.emplace_back(
        Input::Key::F3, Input::State::JustPressed, [&dispatcher]() {
            dispatcher.enqueue<ToggleGuiEvent>();
        });
    static bool loadedMeshes = false;
    if (!loadedMeshes) {
        loadedMeshes = true;

        FSNavigator fd;
        fd.goToFolder("Meshes");
        for (auto& file : fd.getFiles()) {
            auto start = std::chrono::high_resolution_clock::now();
            renderer.loadGLTF(fd.getFilePath(file));
            auto end = std::chrono::high_resolution_clock::now();
            Debug::Info("File: {} Loaded in: {}ms", file.filename().string(), std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
        }
    }

}

void EditorGuiSystem::update(entt::registry& registry, const float deltaTime)
{
    auto&& group = registry.group_if_exists<imp::InputStateComponent>();
    //auto& renderer = registry.ctx().get<CtxRef<Imp::Render::Renderer>>().get();
    if (!group.empty()) {
        auto&& input = registry.try_get<imp::InputStateComponent>(group[0]);
        for (auto&& [key, state, action] : actions) {
            if (input->keyStates[key] == state) {
                action();
            }
        }
    }
    if (gui) {
        drawAll(registry);
    }
}

void EditorGuiSystem::cleanup(entt::registry& registry) { System::cleanup(registry); }
