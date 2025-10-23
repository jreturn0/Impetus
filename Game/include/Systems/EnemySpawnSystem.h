#pragma once
#include "System.h"
class EnemySpawnSystem :public System
{
    void update(entt::registry& registry, const float deltaTime) override;
};

