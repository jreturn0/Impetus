#pragma once
#include "BitFlags.h"
enum HealthDamageLayers {
    None = 0,
    Player1 = 1 << 0,
    Enemy2 = 1 << 1,
    Environment3 = 1 << 2,
    Layer4 = 1 << 3, // Custom layers for future use
    Layer5 = 1 << 4,
    Layer6 = 1 << 5,
    Layer7 = 1 << 6,
    Layer8 = 1 << 7,
    All = Player1 | Enemy2 | Environment3 | Layer4 | Layer5 | Layer6 | Layer7 | Layer8
};

using HealthDamageFlags = utl::BitFlags<HealthDamageLayers>;