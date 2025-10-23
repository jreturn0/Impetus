#pragma once
#include "System.h"
#include <vector>

class ProjectileSystem : public System
{
public:
    void update(entt::registry& registry, const float deltaTime) override;
private:
    std::vector <entt::entity> m_toBeDestroyed{};
};
