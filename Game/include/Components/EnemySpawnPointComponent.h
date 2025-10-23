#pragma once
#include <string>

struct EnemySpawnPointComponent
{
    float spawnInterval{ 1.0f };
    float spawnIntervalTimer{ 0.0f };
    int maxSpawnPerInterval{ 1 };
    int maxEnemies{ 5 };     
    int defaultHealth{ 100 };
    int defaultDamage{ 2 };
    float defaultAccel{ 5000.f };
    float defaultMaxSpeed{ 200.f };
    float defaultSize{ 1.f };
    std::string enemyModelFile{ "basic_shapes.glb" };
    std::string enemyMeshName{ "UVSphere" };
    std::string enemyMaterialName{ "" };
};