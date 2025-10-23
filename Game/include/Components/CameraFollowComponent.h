#pragma once
#include <glm/gtx/quaternion.hpp>
#include <glm/vec3.hpp>

struct CameraFollowComponent
{
    glm::vec3 offset = glm::vec3{0.f};    
    float smoothSpeed = 5.f;
    bool lockY = true;
    float yHeight = 2.f;
    bool orbit = false;
    float orbitSpeed = 1.f;
    float orbitDistance = 5.f;
};
