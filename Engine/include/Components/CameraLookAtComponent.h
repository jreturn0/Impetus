#pragma once
namespace imp {
    struct CameraLookAtComponent {
        float smoothing{ 1.f };
        glm::vec3 previousTargetPosition{ 0.f,0.f,0.f };
    };
}