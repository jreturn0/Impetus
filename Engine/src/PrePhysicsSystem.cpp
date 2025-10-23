#include "Components/PhysicsBodyComponent.h"
#include "Components/TransformComponent.h"
#include "ConfigSystem.h"
#include "CoreSystems/PrePhysicsSystem.h"
#include "CtxRef.h"
#include "events/BasicEvents.h"
#include "Impetus.h"
#include "Physics/Physics.h"
#include <execution>
namespace {
    auto cfg_physicsOptimizeInterval = utl::ConfigValueRef("physics.optimize_interval", 5); //seconds
}

void imp::PrePhysicsSystem::initialize(entt::registry& registry)
{
    m_physicsEngine = &registry.ctx().get<CtxRef<phys::Physics>>().get();
    m_physicsEngine->getSystem().OptimizeBroadPhase();

}

void imp::PrePhysicsSystem::update(entt::registry& registry, const float deltaTime)
{
    auto group = registry.group<>(entt::get<TransformComponent, PhysicsBodyComponent>);
    auto& bodyInterface = m_physicsEngine->getBodyInterface();

    m_optimizeTimer += static_cast<size_t>(deltaTime * 1000);
    if (m_optimizeTimer >= cfg_physicsOptimizeInterval.getCopy() * 1000) {
        m_physicsEngine->getSystem().OptimizeBroadPhase();
        m_optimizeTimer = 0;
    }

    for (auto&& [entity, transform, physicsBody] : group.each()) {
        if (physicsBody.movementType != phys::MovementType::Dynamic) {
            bodyInterface.SetPositionAndRotation(
                physicsBody.id,
                phys::ToJPH(transform.position),
                phys::ToJPH(transform.rotation),
                JPH::EActivation::DontActivate
            );
        }
        if (physicsBody.movementType != phys::MovementType::Static) {
            bodyInterface.SetLinearAndAngularVelocity(
                physicsBody.id,
                phys::ToJPH(physicsBody.linearVelocity),
                phys::ToJPH(physicsBody.angularVelocity)
            );
        }
    }
}

