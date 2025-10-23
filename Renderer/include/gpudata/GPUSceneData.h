#pragma once
#include <glm/mat4x4.hpp>
namespace imp::gfx {
    // Structure to hold scene data for GPU
    struct GPUSceneData
    {
        glm::mat4 inverseView{};
        glm::mat4 proj{};
        glm::mat4 viewproj{};
        glm::vec4 ambientColor{};
        glm::vec4 sunlightDirection{}; // w for sun power
        glm::vec4 sunlightColor{};
    };

}