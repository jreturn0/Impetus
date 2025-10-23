#pragma once
#include "Renderer.h"


// Easy access and serializable component for scene data
// Only use one
struct RenderSceneDataComponent
{
    glm::vec4 ambientColor{1.f};
    glm::vec4 sunlightColor{1.f};  
    glm::vec4 sunlightDirection{1.f,0.f,0.f,0.f}; 
    glm::vec4 computePush1{ 1.f,0.f,0.f,1.f };
    glm::vec4 computePush2{ 0.f,1.f,0.f,1.f };
};