#pragma once
#include <glm/gtx/quaternion.hpp>
#include <glm/vec3.hpp>
#include <entt/entt.hpp>
#include "Debug.h"

namespace imp
{
    struct TransformComponent
    {
        glm::vec3 position{ 0.f };
        glm::quat rotation{ 1.f,0.f,0.f,0.f };
        glm::vec3 scale{ 1.f };
    };

    inline glm::vec3 GetTransformForward(const TransformComponent& transform) { return glm::rotate(transform.rotation, glm::vec3(0, 0, -1)); }
    inline glm::vec3 GetTransformRight(const TransformComponent& transform) { return glm::rotate(transform.rotation, glm::vec3(1, 0, 0)); }
    inline glm::vec3 GetTransformUp(const TransformComponent& transform) { return glm::rotate(transform.rotation, glm::vec3(0, 1, 0)); }
    inline  glm::vec3 GetWorldForward() noexcept { return { 0.f, 0.f, -1.f }; }
    inline  glm::vec3 GetWorldRight() noexcept { return { 1.f, 0.f, 0.f }; }
    inline  glm::vec3 GetWorldUp() noexcept { return { 0.f, 1.f, 0.f }; }

}
