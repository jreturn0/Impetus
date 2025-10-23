﻿#pragma once
#include "System.h"

class AimingSystem : public System
{
public:
	void initialize(entt::registry& registry) override;
	void update(entt::registry& registry, const float deltaTime) override;
};
