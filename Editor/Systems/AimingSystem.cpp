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
		registry.emplace<imp::TransformComponent>(entity);
	}
}

void AimingSystem::update(entt::registry& registry, const float deltaTime)
{
	auto view = registry.view<AimTarget, imp::TransformComponent>();
	auto&& physics = registry.ctx().get<CtxRef<imp::Phys::Physics>>().get();

	auto&& camGroup = registry.group<>(entt::get<imp::TransformComponent, imp::ActiveCameraTag>);
	auto&& renderer = registry.ctx().get<CtxRef<imp::gfx::Renderer>>().get();
	auto&& sceneData = renderer.getSceneData();
	auto&& inputView = registry.view<imp::InputStateComponent>();
	auto&& input = inputView.get<imp::InputStateComponent>(inputView.front());

	camGroup.each([&](auto entity, auto&& transform) {
		auto& aimTarget = registry.get<imp::TransformComponent>(view.front());

		if (auto hit = physics.getMouseWorldPosition(input.getMousePosition(), sceneData.view, sceneData.proj, renderer.getWindow().getSizeVec()))
			aimTarget.position = hit.point + (hit.normal*2.f);
			//Debug::Info("Hit: {}", entt::to_integral(hit.entity));
		
				  });



}

void AimingSystem::cleanup(entt::registry& registry) { System::cleanup(registry); }
