#include "Components/EnemySpawnPointComponent.h"
#include "Components/GameTagComponents.h"
#include "Components/TransformComponent.h"
#include "HealthDamageLayers.h"
#include "Systems/EnemySpawnSystem.h"
#include <Components/CollisionInfoComponent.h>
#include <Components/DamageComponent.h>
#include <Components/HealthComponent.h>
#include <Components/ModelComponent.h>
#include <Components/MoveDirectionComponent.h>
#include <Components/PhysicsBodyComponent.h>
#include <Components/SpeedComponent.h>
#include <Components/TargetComponent.h>
void EnemySpawnSystem::update(entt::registry& registry, const float deltaTime)
{

    auto enemySpawnView = registry.view<EnemySpawnPointComponent, imp::TransformComponent>();
    if (enemySpawnView.begin() == enemySpawnView.end()) return;
    auto enemyGroup = registry.group<>(entt::get<EnemyTag, imp::TransformComponent>);
    int currentEnemyCount = std::distance(enemyGroup.begin(), enemyGroup.end());
    for (auto [entity, spawnPoint, transform] : enemySpawnView.each()) {
        spawnPoint.spawnIntervalTimer += deltaTime;
        if (spawnPoint.spawnIntervalTimer >= spawnPoint.spawnInterval && currentEnemyCount < spawnPoint.maxEnemies) {
            // Spawn an enemy
            //TODO: Some sort of prefab system/resource would be nice here
            auto enemyEntity = registry.create();
            registry.emplace<EnemyTag>(enemyEntity);
            registry.emplace<imp::TransformComponent>(enemyEntity, transform.position, glm::quat{1.f,0.f,0.f,0.f}, glm::vec3(spawnPoint.defaultSize));
            registry.emplace<imp::ModelComponent>(enemyEntity, spawnPoint.enemyModelFile.c_str(), spawnPoint.enemyMeshName.c_str(), spawnPoint.enemyMaterialName.c_str());
            registry.emplace<HealthComponent>(enemyEntity, spawnPoint.defaultHealth, spawnPoint.defaultHealth, HealthDamageLayers::Player1);
            registry.emplace<SpeedComponent>(enemyEntity, spawnPoint.defaultAccel, spawnPoint.defaultMaxSpeed);
            registry.emplace<DamageComponent>(enemyEntity, spawnPoint.defaultDamage, HealthDamageLayers::Enemy2);
            registry.emplace<MoveDirectionComponent>(enemyEntity);
            registry.emplace<imp::CollisionInfoComponent>(enemyEntity);
            registry.emplace<imp::PhysicsBodyComponent>(enemyEntity, imp::phys::Shape{ imp::phys::ShapeType::Sphere,glm::vec3{spawnPoint.defaultSize} }, imp::phys::MovementType::Dynamic, imp::phys::ObjectLayers::DEFAULT);
            auto playerView = registry.view<PlayerTag>();
            registry.emplace<TargetComponent>(enemyEntity, playerView.begin() != playerView.end() ? *playerView.begin() : entt::null);
            spawnPoint.spawnIntervalTimer = 0.0f;
            currentEnemyCount++;
        }
    }

}
