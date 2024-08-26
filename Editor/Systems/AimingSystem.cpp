#include "AimingSystem.h"

#include "CtxRef.h"
#include "../Components/GameTagComponents.h"
#include "Components/TransformComponent.h"
#include "Physics/Physics.h"
#include <Jolt/Physics/Collision/RayCast.h>

#include "Renderer.h"
#include "Components/InputStateComponent.h"
#include "Components/Tags.h"
#include "window/Window.h"

void AimingSystem::initialize(entt::registry& registry)
{
	if (registry.view<AimTarget>().empty()) {
		const auto entity = registry.create();

		registry.emplace<AimTarget>(entity);
		registry.emplace<Imp::TransformComponent>(entity);
	}
}

void AimingSystem::update(entt::registry& registry, const float deltaTime)
{
	auto view = registry.view<AimTarget, Imp::TransformComponent>();
	auto&& physics = registry.ctx().get<CtxRef<Imp::Phys::Physics>>().get();

	auto&& camGroup = registry.group<>(entt::get<Imp::TransformComponent, Imp::ActiveCameraTag>);
	auto&& renderer = registry.ctx().get<CtxRef<Imp::Render::Renderer>>().get();
	auto&& sceneData = renderer.getSceneData();
	auto&& inputView = registry.view<Imp::InputStateComponent>();
	auto&& input = inputView.get<Imp::InputStateComponent>(inputView.front());

	camGroup.each([&](auto entity, auto&& transform) {
		auto& aimTarget = registry.get<Imp::TransformComponent>(view.front());

		if (auto hit = physics.getMouseWorldPosition(input.getMousePosition(), sceneData.view, sceneData.proj, renderer.getWindow().getSize())) 
			aimTarget.position = hit.point + (hit.normal*2.f);
			//Debug::Info("Hit: {}", entt::to_integral(hit.entity));
		
				  });



}

void AimingSystem::cleanup(entt::registry& registry) { System::cleanup(registry); }
