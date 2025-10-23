#include "Components/AimTargetComponent.h"
#include "Components/AttackComponent.h"
#include "Components/CollisionInfoComponent.h"
#include "Components/DamageComponent.h"
#include "Components/GameTagComponents.h"
#include "Components/InputStateComponent.h"
#include "Components/ModelComponent.h"
#include "Components/PhysicsBodyComponent.h"
#include "Components/ProjectileWeaponComponent.h"
#include "Components/TransformComponent.h"
#include "CtxRef.h"
#include "Physics/Physics.h"
#include "Systems/ShootingSystem.h"
#include <Components/LightComponent.h>

void ShootingSystem::update(entt::registry& registry, const float deltaTime)
{
	auto&& aimView = registry.group<>(entt::get<AimTargetComponent, imp::TransformComponent>);
	if (aimView.empty()) return;
	auto&& playerGroup = registry.group<>(entt::get<PlayerTag, imp::TransformComponent, AttackComponent, ProjectileWeaponComponent >);
	if (playerGroup.empty()) return;

	auto&& inputGroup = registry.view<imp::InputStateComponent>();
	auto&& input = inputGroup.get<imp::InputStateComponent>(inputGroup.front());
	auto&& aimTransform = aimView.get<imp::TransformComponent>(aimView.front());
	auto&& [playerTransform, attackCooldown,weapon] = playerGroup.get<imp::TransformComponent, AttackComponent, ProjectileWeaponComponent>(playerGroup.front());
	if (!attackCooldown.canAttack)return;
	constexpr std::string_view file{ "basic_shapes.glb" }, mesh{ "UVSphere" }, material{ "door" };


	if (Input::IsJustPressed(input.mouseButtonStates[Input::MouseButton::Left])) {
		auto direction = glm::normalize(aimTransform.position - playerTransform.position);
		auto lookQuat = glm::quatLookAt(direction, glm::vec3(0, 1, 0));
		auto bullet = registry.create();
		//TODO: Once again, prefabs or similar would be nice here
		registry.emplace<imp::TransformComponent>(bullet, playerTransform.position+(direction*2.5f),lookQuat,glm::vec3{0.25f});
		registry.emplace<ProjectileTag>(bullet);
		registry.emplace<DamageComponent>(bullet, weapon.projectileDamage,weapon.damageFlags);
		registry.emplace<imp::ModelComponent>(bullet, file.data(), mesh.data(), material.data());
		registry.emplace<imp::LightComponent>(bullet,false,glm::vec3{1.f,1.f,1.f});
		registry.emplace<imp::CollisionInfoComponent>(bullet);
		auto&& body =registry.emplace<imp::PhysicsBodyComponent>(bullet, imp::phys::Shape{ imp::phys::ShapeType::Sphere,glm::vec3{0.25f}},imp::phys::MovementType::Dynamic,imp::phys::ObjectLayers::DEFAULT,direction*weapon.projectileSpeed);
		registry.ctx().get<CtxRef<imp::phys::Physics>>().get().getBodyInterface().SetGravityFactor(body.id, 0.f);
		attackCooldown.canAttack = false;
	}



}


