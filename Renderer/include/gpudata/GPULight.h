#pragma once
#include <glm/vec4.hpp>
namespace imp::gfx {

    struct GPULight
    {
        glm::vec4 dirPos{ 1.f }; // w determines type: 0 = directional, 1 = point
        glm::vec4 colour{ 1.f }; // w for power/intensity
        glm::vec4 attenuation{ 1.f, 0.f, 0.f, 0.f }; // constant, linear, quadratic, range for point lights
    };

    struct alignas(16) GPULightData
    {
        static constexpr size_t MaxLights = 64;
        GPULight lights[MaxLights]{};
        uint32_t lightCount{ 0 };
        glm::vec3 padding{}; // Padding to align to 16 bytes
    };
};