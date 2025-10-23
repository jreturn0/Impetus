#pragma once
#include "CameraType.h"
#include <glm/gtx/quaternion.hpp>
#include <glm/mat4x4.hpp>
namespace imp
{
    struct CameraComponent
    {
        float fov{ 85.f };
        float nearPlane{ 0.01f };
        float farPlane{ 1000.f };
        CameraType type{ CameraType::Perspective };
    };
}
