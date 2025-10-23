#pragma once
#include "StringHash.h"
#include "HealthDamageLayers.h"

struct ProjectileWeaponComponent
{
    float projectileSpeed{ 20.f };
    float projectileLifeTime{ 5.f };
    int projectileDamage{ 25 };
    HealthDamageFlags damageFlags{ HealthDamageLayers::All};
};