
#include "ShootingSystem.h"

#include "CtxRef.h"
#include "../Components/AttackComponent.h"
#include "../Components/DamageComponent.h"
#include "../Components/GameTagComponents.h"
#include "Components/CollisionInfoComponent.h"
#include "Components/InputStateComponent.h"
#include "Components/ModelComponent.h"
#include "Components/PhysicsBodyComponent.h"
#include "Components/TransformComponent.h"
#include "Physics/Physics.h"

void ShootingSystem::update(entt::registry& registry, const float deltaTime)
{
	auto&& aimView = registry.group<>(entt::get<AimTarget, imp::TransformComponent>);
	if (aimView.empty()) return;
	auto&& playerGroup = registry.group<>(entt::get<PlayerTag, imp::TransformComponent, AttackComponent>);
	if (playerGroup.empty()) return;

	auto&& inputGroup = registry.view<imp::InputStateComponent>();
	auto&& input = inputGroup.get<imp::InputStateComponent>(inputGroup.front());
	auto&& aimTransform = aimView.get<imp::TransformComponent>(aimView.front());
	auto&& [playerTransform, attackCooldown] = playerGroup.get<imp::TransformComponent, AttackComponent>(playerGroup.front());

	if (!attackCooldown.canAttack)return;
	constexpr std::string_view file{ "basic_shapes.glb" }, mesh{ "UVSphere" }, material{ "Error" };
	if (Input::IsJustPressed(input.mouseButtonStates[Input::MouseButton::Left])) {
		auto bullet = registry.create();
		auto direction = glm::normalize(aimTransform.position - playerTransform.position);
		auto bulletSpeed = 20.f;
		auto lookQuat = glm::quatLookAt(direction, glm::vec3(0, 1, 0));
		registry.emplace<imp::TransformComponent>(bullet, playerTransform.position+(direction*2.5f),lookQuat,glm::vec3{0.25f});
		registry.emplace<ProjectileTag>(bullet);
		registry.emplace<DamageComponent>(bullet, 34);
		registry.emplace<imp::ModelComponent>(bullet, file.data(), mesh.data(), material.data());
		registry.emplace<imp::CollisionInfoComponent>(bullet);
		auto&& body =registry.emplace<imp::PhysicsBodyComponent>(bullet, imp::Phys::Shape{ imp::Phys::ShapeType::Sphere,glm::vec3{0.25f}},imp::Phys::MovementType::Dynamic,imp::Phys::ObjectLayers::DEFAULT,direction*bulletSpeed);
		//attackCooldown.canAttack = false;
		attackCooldown.attackCooldownTimer= attackCooldown.attackCooldown;
		registry.ctx().get<CtxRef<imp::Phys::Physics>>().get().getBodyInterface().SetGravityFactor(body.id, 0.f);

	}



}


