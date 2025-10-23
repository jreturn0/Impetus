#pragma once
#include "PhysicsCommon.h"
#include <entt/entity/entity.hpp>

namespace imp::phys {
    // A struct to hold the result of a raycast
    struct RayHit
    {
        bool hit{};
        entt::entity entity{ entt::null };
        JPH::BodyID body{};
        glm::vec3 point{};
        glm::vec3 normal{};
        float fraction{ -1 };
        operator bool() const
        {
            return hit;
        }

    };
}