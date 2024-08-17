#pragma once
#include "System.h"

class MovementSystem : public System
{
public:
	void update(entt::registry& registry, const float deltaTime) override;
};
