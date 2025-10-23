#pragma once
#include <glm/vec3.hpp>

struct AimTargetComponent
{
    glm::vec3 offset{ 0.0f, 0.0f, 0.0f };
    float normalMultiplier{ 2.f };
    bool lockY{ true };
    float yHeight{ 2.0f };
    bool useCameraPlayerRelative{ true };
};