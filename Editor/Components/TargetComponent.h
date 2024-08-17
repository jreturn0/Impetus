#pragma once
#include <entt/entity/entity.hpp>

struct TargetComponent
{
	entt::entity targetEntity{};
	float distanceToTarget = 0.0f;
};
