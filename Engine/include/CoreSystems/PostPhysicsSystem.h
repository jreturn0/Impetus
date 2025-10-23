#pragma once
#include "System.h"

namespace imp
{
	namespace phys
	{
		class Physics;
	}

	class PostPhysicsSystem final : public System
	{
	public:
		virtual void initialize(entt::registry& registry) override;
		virtual void update(entt::registry& registry, const float deltaTime) override;

	private:
		phys::Physics* m_physicsEngine = nullptr;
	};
}
