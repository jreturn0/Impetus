#include "MovementSystem.h"

#include "CtxRef.h"
#include "Components/TransformComponent.h"
#include "../Components/MoveDirectionComponent.h"
#include "../Components/SpeedComponent.h"
#include "Components/PhysicsBodyComponent.h"
#include "Physics/Physics.h"

void MovementSystem::update(entt::registry& registry, const float deltaTime)
{
	auto group = registry.group <MoveDirectionComponent, SpeedComponent>(entt::get<imp::PhysicsBodyComponent>);

	auto&& bodyInterface = registry.ctx().get<CtxRef<imp::Phys::Physics>>().get().getSystem().GetBodyInterfaceNoLock();

	imp::ForEachParallel(group, [deltaTime, &bodyInterface, &group](auto entity) {
		auto&& [direction, speed, body] = group.get<MoveDirectionComponent, SpeedComponent, imp::PhysicsBodyComponent>(entity);
		JPH::Vec3 lv, av, p;
		JPH::Quat r;
		bodyInterface.GetPositionAndRotation(body.id, p, r);
		bodyInterface.GetLinearAndAngularVelocity(body.id, lv, av);

		auto d = imp::Phys::ToJPH(direction.direction);

		// Add force to move body in direction of d with speed of speed.maxSpeed
		// accelerating by force = (targetVelocity - currentVelocity) * speed.speed * deltaTime
		auto tlv = d * speed.maxSpeed;
		auto dlv = tlv - lv;
		auto f = dlv * speed.speed * deltaTime;

		if (direction.faceDirection) {
			// Calculate desired rotation quaternion to face the direction of movement
			JPH::Vec3 forward = r * JPH::Vec3(0.f, 0.f, 1.f);
			auto tr = JPH::Quat::sFromTo(forward, d);

			JPH::Vec3 axis;
			float angle;
			tr.GetAxisAngle(axis, angle);

			// Compute the angular velocity needed to rotate towards the desired direction
			auto t = axis * angle * deltaTime;  // Angular force (torque) scaled by deltaTime
			bodyInterface.AddForceAndTorque(body.id, f, t);
		}
		else {
			bodyInterface.AddForce(body.id, f);
		}

						 });
}
