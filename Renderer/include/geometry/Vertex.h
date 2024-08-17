#pragma once

#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
namespace Imp::Render {
    struct Vertex
    {
        glm::vec3 position;
        float uv_x;
        glm::vec3 normal;
        float uv_y;
        glm::vec4 color;

        //static vk::VertexInputBindingDescription getBindingDescription()
        //{
        //    return {0, sizeof(Vertex), vk::VertexInputRate::eVertex};
        //}
        //using VertexInputAttributeDescriptions = std::array<vk::VertexInputAttributeDescription, 2>;
        //static VertexInputAttributeDescriptions getAttributeDescriptions()
        //{
        //    return VertexInputAttributeDescriptions{ {
        //        {0, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, position)},
        //        {1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color)}
        //    } };
        //}

    };
}


const std::vector<Imp::Render::Vertex> TEST_VERTICES = {
    //{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    //{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    //{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    //{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};

const std::vector<uint16_t> TEST_INDICES = {
    0, 1, 2, 2, 3, 0
};