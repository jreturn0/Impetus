#pragma once
#include "System.h"

namespace imp
{
	namespace Phys
	{
		class Physics;
	}

	class PostPhysicsSystem final : public System
	{
	private:
		Phys::Physics* physicsSystem = nullptr;
	public:
		virtual void initialize(entt::registry& registry) override;
		virtual void update(entt::registry& registry, const float deltaTime) override;
		virtual void cleanup(entt::registry& registry) override;

		
	};
}
