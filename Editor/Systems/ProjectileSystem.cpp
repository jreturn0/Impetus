#include "ProjectileSystem.h"

#include "CtxRef.h"
#include "../Components/DamageComponent.h"
#include "../Components/GameTagComponents.h"
#include "../Components/HealthComponent.h"
#include "Components/CollisionInfoComponent.h"
#include "Components/TransformComponent.h"
#include "BasicEvents.h"
#include "FixedQueue.h"

void ProjectileSystem::update(entt::registry& registry, const float deltaTime)
{
	auto&& group = registry.group<ProjectileTag>(entt::get<imp::TransformComponent, DamageComponent, imp::CollisionInfoComponent>);
	FixedQueue<entt::entity, 64> queue{};
	imp::ForEachParallel(group, [&registry, &group, &queue](auto entity) {
		auto&& [transform, damage, collisionInfo] = group.get<imp::TransformComponent, DamageComponent, imp::CollisionInfoComponent>(entity);

		if (!collisionInfo.collidingEntities.empty()) {
			queue.enqueue(entity);
		} else {
			return;
		}
		for (auto&& collision : collisionInfo.collidingEntities) {

			if (registry.valid(collision)) {
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
