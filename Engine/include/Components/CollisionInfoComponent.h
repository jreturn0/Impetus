#pragma once
#include <vector>
#include <entt/container/dense_set.hpp>
#include <entt/entity/entity.hpp>

namespace Imp {



	struct CollisionInfoComponent
	{

		entt::dense_set<entt::entity> collidingEntities; //@skip


	};

}
