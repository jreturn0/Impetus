#pragma once
#include "System.h"
class AttackSystem : public System
{
    void update(entt::registry& registry, const float deltaTime) override;
};

