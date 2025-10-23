#pragma once

#include "System.h"


class CameraFollowSystem : public System
{
    void update(entt::registry& registry, const float deltaTime) override;
};