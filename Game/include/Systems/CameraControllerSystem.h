#pragma once
#include "System.h"


class CameraControllerSystem final : public System
{
    void initialize(entt::registry& registry) override;
    void update(entt::registry& registry, const float deltaTime) override;
};
