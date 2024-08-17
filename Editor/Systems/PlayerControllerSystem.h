#pragma once
#include "System.h"

class PlayerControllerSystem final : public System
{
public:
	void update(entt::registry& registry, const float deltaTime) override;
};
