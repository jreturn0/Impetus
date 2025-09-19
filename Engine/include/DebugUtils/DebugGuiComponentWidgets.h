#pragma once
#ifdef _DEBUG
#include <imgui.h>
#include <entt/entity/registry.hpp>

#include "Components/ModelComponent.h"


#include "Renderer.h"
#include <CtxRef.h>


inline void ModelWidgetInfo(entt::registry& registry, entt::entity entity)
{
	auto& model = registry.get<imp::ModelComponent>(entity);
	auto& renderer = registry.ctx().get<CtxRef<imp::gfx::Renderer>>().get();

	auto& modelNames = renderer.getLoadedGLTFNames();
	if (modelNames.size() == 0) {
		ImGui::Text("No models loaded");
		return;
	}
	ImGui::Text("Model: ");
	ImGui::SameLine();
	if (ImGui::Button(model.gltf.data())) {
		ImGui::OpenPopup("GLTFModel");
	}

	if (ImGui::BeginPopup("GLTFModel")) {
		for (auto& name : modelNames) {
			if (ImGui::Selectable(name.data())) {
				model.gltf = name;
				model.name = "Root";
			}
		}
		ImGui::EndPopup();
	}

	if (auto& nodeNames = renderer.getLoadedGLTFNodeNames(model.gltf); nodeNames.size() > 0) {
		ImGui::Text("Node: ");
		ImGui::SameLine();
		if (ImGui::Button(model.name.data())) {
			ImGui::OpenPopup("GLTFNode");
		}

		if (ImGui::BeginPopup("GLTFNode")) {
			for (auto& name : nodeNames) {
				if (ImGui::Selectable(name.data())) {
					model.name = name;
				}
			}
			ImGui::EndPopup();
		}
	}

	auto& materialNames = renderer.getMaterialNames();

	if (materialNames.size() > 0) {
		ImGui::Text("Material: ");
		ImGui::SameLine();
		if (ImGui::Button(model.materialOverrideName.data())) {
			ImGui::OpenPopup("Material");
		}

		if (ImGui::BeginPopup("Material")) {
			for (auto& name : materialNames) {
				if (ImGui::Selectable(name.data())) {
					model.materialOverrideName = name;
				}
			}
			ImGui::EndPopup();
		}
	}
}
#endif