#pragma once
#include <entt/entt.hpp>

namespace imp {
	struct CollisionInfoComponent
	{
		entt::dense_set<entt::entity> collidingEntities{};
	};
}
