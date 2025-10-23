#include "events/BasicEvents.h"
#include "Components/CollisionInfoComponent.h"
#include "Components/DamageComponent.h"
#include "Components/GameTagComponents.h"
#include "Components/HealthComponent.h"
#include "Components/TransformComponent.h"
#include "CtxRef.h"
#include "FixedQueue.h"
#include "Systems/ProjectileSystem.h"
#include <ThreadPool.h>


namespace imp {
    template<typename T, typename F>
    void ForEachParallel(utl::ThreadPool& pool, T& container, F&& task)
    {
        // Split and batch process tasks based on the number of threads in the pool and the size of the container
        size_t totalItems = container.size();
        size_t threadCount = pool.threadCount();

        if (totalItems == 0 || threadCount == 0) {
            return;
        }

        size_t batchSize = (totalItems + threadCount - 1) / threadCount; // Ceiling division to ensure all items are processed
        // Example: (135 items + 8 threads 

    }
}

void ProjectileSystem::update(entt::registry& registry, const float deltaTime)
{
    auto&& group = registry.group<ProjectileTag>(entt::get<imp::CollisionInfoComponent>);
    //auto&& group = registry.view<ProjectileTag, imp::CollisionInfoComponent>();
    m_toBeDestroyed.clear();
    group.each([this, &registry](auto entity, auto& collisionInfo) {
        if (!collisionInfo.collidingEntities.empty())
            m_toBeDestroyed.push_back(entity);

        });


    for (auto entity : m_toBeDestroyed)
    {
        if (registry.valid(entity))
            registry.destroy(entity);
    }
}
