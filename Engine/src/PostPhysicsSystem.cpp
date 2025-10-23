#include "Components/CollisionInfoComponent.h"
#include "Components/PhysicsBodyComponent.h"
#include "Components/RelationshipComponent.h"
#include "Components/TransformComponent.h"
#include "CoreSystems/PostPhysicsSystem.h"
#include "CtxRef.h"
#include "events/BasicEvents.h"
#include "Physics/Physics.h"
#include <execution>
#include "ThreadPool.h"
void imp::PostPhysicsSystem::initialize(entt::registry& registry)
{
    m_physicsEngine = &registry.ctx().get<CtxRef<phys::Physics>>().get();

}

void imp::PostPhysicsSystem::update(entt::registry& registry, const float deltaTime)
{
    const auto& group = registry.group<PhysicsBodyComponent>();
    auto& physicsPrev = registry.storage<TransformComponent>("physics_prev"_hs);
    auto& physicsCurr = registry.storage<TransformComponent>("physics"_hs);
    physicsPrev.clear();
    physicsPrev.reserve(physicsCurr.size());
    for (auto [entity, curr] : physicsCurr.each()) {
        physicsPrev.emplace(entity, curr); // copy
    }

    physicsCurr.clear();
    physicsCurr.reserve(group.size());

    auto& bodyInterface = m_physicsEngine->getBodyInterface();

    for (auto [entity, body] : registry.group<imp::PhysicsBodyComponent>().each()) {
        JPH::Vec3 pos; JPH::Quat rot;
        bodyInterface.GetPositionAndRotation(body.id, pos, rot);

        imp::TransformComponent t;
        t.position = phys::ToGLM(pos);
        t.rotation = phys::ToGLM(rot);
        physicsCurr.emplace(entity, std::move(t));

        JPH::Vec3 lin, ang;
        bodyInterface.GetLinearAndAngularVelocity(body.id, lin, ang);
        body.linearVelocity = phys::ToGLM(lin);
        body.angularVelocity = phys::ToGLM(ang);
    }

}

