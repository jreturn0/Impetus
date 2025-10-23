#pragma once
#include <glm/gtc/quaternion.hpp>

struct CameraControllerComponent {
    float pitch{ 0.f };
    float yaw{ 0.f };
    bool smooth{ true };
    float smoothSpeed{ 5.f };
    float targetPitch{ 0.f };
    float targetYaw{ 0.f };
    float moveSpeed{ 5.f };
    float moveBoostSpeed{ 50.f };
};
