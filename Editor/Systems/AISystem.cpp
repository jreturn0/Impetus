#include "AISystem.h"

#include "CtxRef.h"
#include "../Components/SpeedComponent.h"
#include "../Components/TargetComponent.h"
#include "../Components/MoveDirectionComponent.h"
#include "Components/PhysicsBodyComponent.h"
#include "Components/TransformComponent.h"
#include "Physics/Physics.h"

struct TargetComponent;


void AISystem::update(entt::registry& registry, const float deltaTime)
{
	auto&& group = registry.group<TargetComponent>(entt::get< MoveDirectionComponent, imp::PhysicsBodyComponent>);
	auto&& bodyInterface = registry.ctx().get<CtxRef<imp::Phys::Physics>>().get().getBodyInterface();
	imp::ForEachParallel(group,
						 [&](auto entity) {
							 auto&& [target, direction, physics] = group.get<TargetComponent, MoveDirectionComponent, imp::PhysicsBodyComponent>(entity);

							 //check if target is still alive
							 if (!registry.valid(target.targetEntity)) {
								 // registry.remove<TargetComponent>(entity);
								 return;
							 }
							 auto pos = imp::Phys::ToGLM(bodyInterface.GetPosition(physics.id));

							 glm::vec3 targetPos;

							 // check and get if target has physics body else get transform component
							 if (auto&& targetPhysics = registry.try_get<imp::PhysicsBodyComponent>(target.targetEntity)) {
								 targetPos = imp::Phys::ToGLM(bodyInterface.GetPosition(targetPhysics->id));
							 } else {
								 auto&& targetTransform = registry.try_get<imp::TransformComponent>(target.targetEntity);
								 if (!targetTransform) {
									 return;
								 }
								 targetPos = targetTransform->position;
							 }



							 // auto& targetTransform = registry.get<Imp::TransformComponent>(target.targetEntity);
							  // Calculate the direction vector towards the target
							 direction.direction = glm::normalize(targetPos - pos);
							 //// Calculate the desired velocity towards the target
							 //auto desiredVelocity = direction * speed.speed;
							 //auto distance = glm::distance2(targetPos, pos);
							 //if (distance <= target.distanceToTarget) {
								// // Scale the velocity down proportionally as it approaches the stopping distance
								// float slowingFactor = distance / target.distanceToTarget;
								// desiredVelocity *= slowingFactor;
							 //}
							 //// Calculate the difference between the desired velocity and the current velocity
							 //auto velocityChange = desiredVelocity - physics.linearVelocity;

							 //// Limit the magnitude of the velocity change to ensure it doesn't exceed maxSpeed
							 //if (glm::length2(velocityChange) > speed.maxSpeed * speed.maxSpeed) {
								// velocityChange = glm::normalize(velocityChange) * speed.maxSpeed;
							 //}



							 //// Update the current velocity by adding the velocity change
							 //physics.linearVelocity += velocityChange;
							 //bodyInterface.SetLinearVelocity(physics.id, Imp::Phys::ToJPH(physics.linearVelocity));

						 }
	);

}
