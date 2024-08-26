#include "EnemySystem.h"

#include <FixedQueue.h>
#include "../Components/DamageComponent.h"
#include "../Components/GameTagComponents.h"
#include "../Components/HealthComponent.h"
#include "Components/CollisionInfoComponent.h"
#include "Components/TransformComponent.h"

void EnemySystem::OnEnemyDeath(entt::registry& registry, entt::entity entity)
{}



void SpawnEnemy(entt::registry& registry)
{
	auto entity = registry.create();

	//auto transform = registry.emplace<Imp::TransformComponent>(entity);
	//auto health = registry.emplace<HealthComponent>(entity);
	//auto damage = registry.emplace<DamageComponent>(entity);
	//auto collisionInfo = registry.emplace<Imp::CollisionInfoComponent>(entity);
	//auto enemyTag = registry.emplace<EnemyTag>(entity);
	//auto body = registry.emplace<Imp::PhysicsBodyComponent>(entity);


}

void EnemySystem::update(entt::registry& registry, const float deltaTime)
{


	auto&& group = registry.group<EnemyTag>(entt::get<HealthComponent, DamageComponent, Imp::CollisionInfoComponent>);

	FixedQueue<entt::entity, 64> queue{};





	Imp::ForEachParallel(group, [&registry, &group, &queue](auto entity) {
		auto&& [health, damage, collisionInfo] = group.get<HealthComponent, DamageComponent, Imp::CollisionInfoComponent>(entity);

		if (health.health <= 0) {
			queue.enqueue(entity);
			return;
		} 
		for (auto&& collision : collisionInfo.collidingEntities) {

			if (registry.valid(collision)) {
				if (registry.all_of<PlayerTag>(collision))
					if (auto&& health = registry.try_get<HealthComponent>(collision)) {
						health->health -= damage.damage;
					}
			}
		}
						 });

	entt::entity entity;
	while (queue.dequeueUnsafe(entity)) {
		registry.destroy(entity);
	}

}
