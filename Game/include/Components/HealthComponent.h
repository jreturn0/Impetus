#pragma once
#include "HealthDamageLayers.h"
#include <entt/entt.hpp>

struct HealthComponent
{
	int health=100;
	int maxHealth=100;
	HealthDamageFlags layers{ HealthDamageLayers::All };
};

