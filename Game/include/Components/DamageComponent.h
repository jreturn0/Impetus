#pragma once
#include "HealthDamageLayers.h"


struct DamageComponent
{
    int damage{ 10 };
    HealthDamageFlags layers{ HealthDamageLayers::All };
};