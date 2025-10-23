#pragma once

#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
namespace imp::gfx {
    // Vertex structure for 3D geometry
    struct Vertex
    {
        glm::vec3 position{};
        float uv_x{};
        glm::vec3 normal{};
        float uv_y{};
        glm::vec4 color{};
    };
}