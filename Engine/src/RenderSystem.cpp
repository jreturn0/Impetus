#include "events/BasicEvents.h"
#include "Components/CameraComponent.h"
#include "Components/InputStateComponent.h"
#include "Components/ModelComponent.h"
#include "Components/RelationshipComponent.h"
#include "Components/TransformComponent.h"
#include "core/ComputeEffect.h"
#include "CoreSystems/RenderSystem.h"
#include "CtxRef.h"
#include "EngineStats.h"
#include "InputCodes.h"
#include "Renderer.h"
#include <glm/gtx/quaternion.hpp>
#include <Components/LightComponent.h>


void imp::RenderSystem::OnToggleGuiEvent()
{
	m_guiEnabled = !m_guiEnabled;
}

void imp::RenderSystem::initialize(entt::registry& registry)
{
	registry.ctx().get<CtxRef<entt::dispatcher>>().get().sink<ToggleGuiEvent>().connect<&RenderSystem::OnToggleGuiEvent>(this);
}

void imp::RenderSystem::update(entt::registry& registry, const float deltaTime)
{
	auto& renderer = registry.ctx().get<CtxRef<imp::gfx::Renderer>>().get();
	auto modelGroup = registry.group<ModelComponent>();

	modelGroup.each([&renderer](auto entity, ModelComponent& model) {
		if (model.visible)
			renderer.draw(model.gltf, model.name, model.localTransform, model.materialOverrideName);
					});

	
	auto lightGroup = registry.group<LightComponent>(entt::get< TransformComponent>);
	lightGroup.each([&renderer](auto entity, LightComponent& light, TransformComponent& transform) {
		imp::gfx::GPULight gpuLight{

		 glm::vec4{ light.isDirectional ? GetTransformForward(transform) : transform.position, light.isDirectional ? 0.0f : 1.0f},
		 glm::vec4{ light.colour,light.intensity},
		 glm::vec4( light.constantAttenuation, light.linearAttenuation, light.quadraticAttenuation, light.range)

		};
		renderer.drawLight(gpuLight);
		});

	renderer.beginFrame();
	renderer.drawCompute();
	renderer.drawGeometry();
	//
#ifdef _DEBUG
	if (m_guiEnabled)
		renderer.startDrawGui();
#endif

}

void imp::RenderSystem::cleanup(entt::registry& registry)
{
	registry.ctx().get<CtxRef<entt::dispatcher>>().get().sink<ToggleGuiEvent>().disconnect(this);
}
