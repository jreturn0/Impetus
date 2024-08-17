#include "CoreSystems/RenderSystem.h"


#include <glm/gtx/quaternion.hpp>

#include "BasicEvents.h"
#include "CtxRef.h"
#include "EngineStats.h"
#include "InputCodes.h"
#include "Renderer.h"
#include "Components/CameraComponent.h"
#include "Components/InputStateComponent.h"
#include "Components/ModelComponent.h"
#include "Components/RelationshipComponent.h"
#include "Components/TransformComponent.h"
#include "core/ComputeEffect.h"


void Imp::RenderSystem::OnToggleGuiEvent()
{
	gui = !gui;
}

void Imp::RenderSystem::initialize(entt::registry& registry)
{
	//observer.connect(registry, entt::collector.update<TransformComponent>());
	registry.ctx().get<CtxRef<entt::dispatcher>>().get().sink<ToggleGuiEvent>().connect<&RenderSystem::OnToggleGuiEvent>(this);

}


//void UpdateModelMatrix(entt::registry& registry, entt::entity entity)
//{
//	auto* transform = registry.try_get<Imp::TransformComponent>(entity);
//	auto* model = registry.try_get<Imp::ModelComponent>(entity);
//
//
//	if (transform && model) {
//		// Compute the local transform
//		glm::mat4 translation = glm::translate(glm::mat4(1.0f), transform->position);
//		glm::mat4 rotation = glm::toMat4(transform->rotation);
//		glm::mat4 scale = glm::scale(glm::mat4(1.0f), transform->scale);
//		model->localTransform = translation * rotation * scale;
//
//		auto* relationship = registry.try_get<Imp::RelationshipComponent>(entity);
//		// Compute the global transform
//		if (relationship && relationship->parent != entt::null) {
//			if (const auto* parentModel = registry.try_get<Imp::ModelComponent>(relationship->parent)) {
//				model->localTransform = parentModel->localTransform * model->localTransform;
//			}
//		} else {
//			model->localTransform = model->localTransform;
//		}
//
//		// Update children
//		if (relationship) {
//			for (auto child : relationship->children) {
//				UpdateModelMatrix(registry, child);
//			}
//		}
//	}
//}


void Imp::RenderSystem::update(entt::registry& registry, const float deltaTime)
{
	auto& renderer = registry.ctx().get<CtxRef<Imp::Render::Renderer>>().get();

	//observer.each([&registry](const auto entity) {
	//	UpdateModelMatrix(registry, entity);
	//			  });
	//observer.clear();
	auto modelGroup = registry.group<ModelComponent>();

	modelGroup.each([&renderer](auto entity, ModelComponent& model) {
		if (model.visible)
			renderer.draw(model.gltf, model.name, model.localTransform, model.materialOverrideName);
					});


	renderer.beginFrame();
	renderer.drawCompute();
	renderer.drawGeometry();
	//
#ifdef _DEBUG
	if (gui)
		renderer.startDrawGui();
#endif

}

void Imp::RenderSystem::cleanup(entt::registry& registry)
{
	registry.ctx().get<CtxRef<entt::dispatcher>>().get().sink<ToggleGuiEvent>().disconnect(this);
	System::cleanup(registry);
}
