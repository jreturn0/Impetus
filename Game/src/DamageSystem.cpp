#include "Components/CollisionInfoComponent.h"
#include "Components/DamageComponent.h"
#include "Components/HealthComponent.h"
#include "CtxRef.h"
#include "Systems/DamageSystem.h"
#include "ThreadPool.h"

void DamageSystem::update(entt::registry& registry, const float deltaTime)
{
    auto&& group = registry.group<DamageComponent>(entt::get<imp::CollisionInfoComponent>);
    std::queue<entt::entity> toRemove{};
    group.each([&registry, &toRemove](auto entity, auto& damage, auto& collisionInfo) {
        for (auto&& collision : collisionInfo.collidingEntities) {
            if (registry.valid(collision)) {
                if (auto* health = registry.try_get<HealthComponent>(collision); health != nullptr) {
                    if (health->layers & damage.layers)
                        health->health -= damage.damage;
                    if (health->health <= 0.f) {
                        toRemove.push(collision);
                    }
                }
            }
        }
        });
    while (!toRemove.empty()) {
        if (auto entity = toRemove.front(); registry.valid(entity))
            registry.destroy(entity);
        toRemove.pop();
    }

}
