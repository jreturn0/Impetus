#pragma once
#include <entt/entity/observer.hpp>

#include "System.h"
#include "Physics/Physics.h"
namespace Imp {


	class PhysicsSystem final : public System
	{
	private:
		Phys::Physics* physicsSystem=nullptr;
		std::queue<std::function<void()>> deferredEvents;
		entt::registry* registry_ = nullptr;
		entt::observer observer;
		
		
		void OnPhysicsBodyComponentConstruct(entt::registry& registry, entt::entity entity);
		void OnPhysicsBodyComponentDestroy(entt::registry& registry, entt::entity entity) const;
		void OnPhysicsBodyComponentUpdate(entt::registry& registry, entt::entity entity);

		void OnCollisionInfoConstruct( entt::registry& registry, entt::entity entity) const;

		void handleCollision (entt::registry& registry,const Imp::Phys::ContactEvent& event);
		//void OnContactAdded(const Phys::CollisionInfo& info);
		//void OnContactPersisted(const Phys::CollisionInfo& info);
		//void OnContactRemoved(std::array<JPH::BodyID, 2> info);
		void OnCollisionEvent(const Imp::Phys::ContactEvent& event);
		std::mutex collisionEventsMutex;
		std::queue<Imp::Phys::ContactEvent> collisionEvents;
	public:
		void initialize(entt::registry& registry) override;
		void update(entt::registry& registry, const float deltaTime) override;
		void cleanup(entt::registry& registry) override;
	};
}
