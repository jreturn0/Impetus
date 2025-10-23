#pragma once
#include "System.h"
class DamageSystem : public System
{
    void update(entt::registry& registry, const float deltaTime) override;
};

