#pragma once
#include "StringHash.h"
#include "TypeName.h"
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <execution>
// A base class for all systems 
class System
{
public:
    virtual ~System() = default;
    virtual void initialize(entt::registry& registry) {};
    virtual void update(entt::registry& registry, const float deltaTime) = 0;
    virtual void cleanup(entt::registry& registry) {};
};

