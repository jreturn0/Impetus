#pragma once
#include <entt/entity/observer.hpp>

#include "System.h"


namespace imp {
	class Clock;
	class TransformSystem final : public System
	{
	public:
		void initialize(entt::registry& registry) override;
		void update(entt::registry& registry, const float deltaTime) override;
	private:
		Clock* time;
	};
}
