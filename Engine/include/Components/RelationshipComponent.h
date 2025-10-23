#pragma once
#include <entt/entity/entity.hpp>
#include <glm/mat4x4.hpp>
#include <vector>
namespace imp
{
    // Unused
    struct RelationshipComponent
    {
        std::size_t children{};
        entt::entity first{ entt::null };
        entt::entity prev{ entt::null };
        entt::entity next{ entt::null };
        entt::entity parent{ entt::null };
    };
}
