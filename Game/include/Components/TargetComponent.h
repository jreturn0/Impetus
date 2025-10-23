#pragma once
#include <entt/entity/entity.hpp>

struct TargetComponent
{
	entt::entity targetEntity{entt::null};
	float distanceToTarget = 0.0f;
};
