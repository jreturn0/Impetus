#include "CoreSystems/PostPhysicsSystem.h"

#include <execution>

#include "CtxRef.h"
#include "Physics/Physics.h"
#include "BasicEvents.h"
#include "Components/CollisionInfoComponent.h"
#include "Components/PhysicsBodyComponent.h"
#include "Components/RelationshipComponent.h"
#include "Components/TransformComponent.h"

void imp::PostPhysicsSystem::initialize(entt::registry& registry)
{
	physicsSystem = &registry.ctx().get<CtxRef<Phys::Physics>>().get();

}

void imp::PostPhysicsSystem::update(entt::registry& registry, const float deltaTime)
{
	auto&& group = registry.group<PhysicsBodyComponent>();

	std::mutex mutex{};
	//auto&& physicsTransforms = registry.storage<TransformComponent>("postPhysics"_hs);
	//physicsTransforms.clear();
	//physicsTransforms.reserve(group.size());
	auto& bodyInterface = physicsSystem->getBodyInterface();
	auto& prePhysicsStorage = registry.storage<TransformComponent>("physics"_hs);
	prePhysicsStorage.clear();
	prePhysicsStorage.reserve(group.size());
	//Debug::Info("Post Physics System Update");

	std::for_each(std::execution::par_unseq, group.begin(), group.end(), [&group, &bodyInterface, &mutex, &prePhysicsStorage](auto entity) {
		auto&& physicsBody = group.get<PhysicsBodyComponent>(entity);
		JPH::Vec3 out1{}, out2;
		JPH::Quat out3{};
		bodyInterface.GetPositionAndRotation(physicsBody.id, out1, out3);
		TransformComponent temp{ Phys::ToGLM(out1),Phys::ToGLM(out3) };
		{
			std::lock_guard<std::mutex> lock(mutex);
			prePhysicsStorage.push(entity, &temp);
		}
		bodyInterface.GetLinearAndAngularVelocity(physicsBody.id, out1, out2);
		physicsBody.linearVelocity = Phys::ToGLM(out1);
		physicsBody.angularVelocity = Phys::ToGLM(out2);



				  });

}





void imp::PostPhysicsSystem::cleanup(entt::registry& registry)
{}
