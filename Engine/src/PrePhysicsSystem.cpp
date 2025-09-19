#include "CoreSystems/PrePhysicsSystem.h"

#include <execution>

#include "CtxRef.h"
#include "Physics/Physics.h"
#include "Components/PhysicsBodyComponent.h"
#include "Components/TransformComponent.h"
#include "BasicEvents.h"
void imp::PrePhysicsSystem::initialize(entt::registry& registry)
{
	physicsSystem = &registry.ctx().get<CtxRef<Phys::Physics>>().get();
	physicsSystem->getSystem().OptimizeBroadPhase();
}

void imp::PrePhysicsSystem::update(entt::registry& registry, const float deltaTime)
{
	auto group = registry.group<>(entt::get<TransformComponent, PhysicsBodyComponent>);
	auto& bodyInterface = physicsSystem->getBodyInterface();
	std::mutex mutex{};

	std::for_each(std::execution::par_unseq, group.begin(), group.end(), [&group, &bodyInterface, &mutex](auto entity) {
		auto&& [transform, physicsBody] = group.get<TransformComponent, PhysicsBodyComponent>(entity);
	


		bodyInterface.SetPositionAndRotation(physicsBody.id, Phys::ToJPH(transform.position), Phys::ToJPH(transform.rotation), JPH::EActivation::DontActivate);
		if (physicsBody.movementType != Phys::MovementType::Static)
			bodyInterface.SetLinearAndAngularVelocity(physicsBody.id, Phys::ToJPH(physicsBody.linearVelocity), Phys::ToJPH(physicsBody.angularVelocity));

				  });

}

void imp::PrePhysicsSystem::cleanup(entt::registry& registry)
{
	/* Debug::Out("PrePhysicsSystem::update: entity: {}, position: {} {} {}, rotation: {} {} {} {}\n",
			entt::to_integral(entity),
			transform.position.x, transform.position.y, transform.position.z,
			transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w);
		});*/
		//}
}
