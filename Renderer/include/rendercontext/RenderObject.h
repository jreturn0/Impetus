#pragma once
#include <glm/mat4x4.hpp>

#include "core/Buffer.h"
#include "geometry/Bounds.h"

namespace Imp::Render {
    struct Material;

    struct RenderObject
    {
        uint32_t indexCount;
        uint32_t firstIndex;
        vk::Buffer indexBuffer;
       // std::string materialName;
        std::shared_ptr<Material> material;
        Bounds bounds;
        glm::mat4 transform;
        vk::DeviceAddress vertexBufferAddress;
    };
}