#pragma once
#include "System.h"

//Used for testing
class MapSystem : public System
{
public:
    virtual void initialize(entt::registry& registry) override;
    virtual void update(entt::registry& registry, const float deltaTime) override {};

private:
    const int mapWidth = 15, mapLength = 15, tileSize = 20;
    std::string_view file{ "basic_shapes.glb" }, mesh{ "Cube" }, material{ "Sand" };
};

