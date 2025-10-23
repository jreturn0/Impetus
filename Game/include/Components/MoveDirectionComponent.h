#pragma once
#include <glm/vec3.hpp>


struct MoveDirectionComponent
{
    glm::vec3 direction{ 0.f };
    bool faceDirection{ true };
};
