#pragma once
#include "System.h"

class ShootingSystem : public System
{
public:
	void update(entt::registry& registry, const float deltaTime) override;
};
