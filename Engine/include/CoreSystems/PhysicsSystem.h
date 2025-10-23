#pragma once
#include "Physics/Physics.h"
#include "System.h"
#include <entt/entity/observer.hpp>

namespace imp {


	class PhysicsSystem final : public System
	{
	public:
		void initialize(entt::registry& registry) override;
		void update(entt::registry& registry, const float deltaTime) override;
		void cleanup(entt::registry& registry) override;
	private:
		phys::Physics* m_physicsEngine{ nullptr };
		std::queue<std::function<void()>> m_pendingEvents{};
		std::mutex m_collisionEventsMutex{};
		std::queue<imp::phys::ContactEvent> m_collisionEvents{};

		void OnPhysicsBodyComponentConstruct(entt::registry& registry, entt::entity entity);
		void OnPhysicsBodyComponentDestroy(entt::registry& registry, entt::entity entity) const;
		void OnPhysicsBodyComponentUpdate(entt::registry& registry, entt::entity entity);
		void OnCollisionInfoConstruct(entt::registry& registry, entt::entity entity) const;
		void OnCollisionEvent(const imp::phys::ContactEvent& event);

		void handleCollision(entt::registry& registry, const imp::phys::ContactEvent& event);



	};
}
