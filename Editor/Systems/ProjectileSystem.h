#pragma once
#include "System.h"

class ProjectileSystem : public System
{
public:
	void update(entt::registry& registry, const float deltaTime) override;
};
