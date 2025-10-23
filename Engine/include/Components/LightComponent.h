#pragma once
#include <glm/vec3.hpp>

namespace imp {
    struct LightComponent {
        bool isDirectional{ false }; // True for directional light, false for point light
        glm::vec3 colour{ 1.f, 1.f, 1.f };
        float intensity{ 1.f };
        // Attenuation factors for point lights
        float constantAttenuation{ 1.f };
        float linearAttenuation{ 0.01f };
        float quadraticAttenuation{ 0.1f };
        float range{ 10.f }; // Effective range for point lights
        bool visible{ true };
    };
};