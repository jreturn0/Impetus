#pragma once
#include "System.h"

class AISystem final : public System
{
public:

	void update(entt::registry& registry, const float deltaTime) override;

};
