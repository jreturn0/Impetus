#pragma once
#include <glm/mat4x4.hpp>
#include "utils/VKCommon.hpp"
namespace imp::gfx
{
    // Structure to represent push constants for GPU draw calls
    struct GPUDrawPushConstants
    {
        glm::mat4 worldMatrix{};
        vk::DeviceAddress vertexBuffer{};
    };
}
