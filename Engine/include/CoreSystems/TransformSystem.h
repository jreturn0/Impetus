#pragma once
#include <entt/entity/observer.hpp>

#include "System.h"

namespace imp
{
	class Clock;
}

namespace imp {
	class TransformSystem final : public System
	{
	private:
		Clock* time;
	public:
		void initialize(entt::registry& registry) override;
		void update(entt::registry& registry, const float deltaTime) override;
		void cleanup(entt::registry& registry) override;
	};
}
