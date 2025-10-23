#include "Components/MoveDirectionComponent.h"
#include "Components/PhysicsBodyComponent.h"
#include "Components/SpeedComponent.h"
#include "Components/TargetComponent.h"
#include "Components/TransformComponent.h"
#include "CtxRef.h"
#include "Physics/Physics.h"
#include "Systems/AISystem.h"
#include "ThreadPool.h"


void AISystem::update(entt::registry& registry, const float deltaTime)
{
    auto&& group = registry.group<TargetComponent>(entt::get< MoveDirectionComponent, imp::PhysicsBodyComponent>);
    auto&& bodyInterface = registry.ctx().get<CtxRef<imp::phys::Physics>>().get().getBodyInterface();
    group.each([&](entt::entity entity, TargetComponent& target, MoveDirectionComponent& direction, imp::PhysicsBodyComponent& physics) {
        if (!registry.valid(target.targetEntity)) {
            return;
        }
        auto pos = imp::phys::ToGLM(bodyInterface.GetPosition(physics.id));

        glm::vec3 targetPos;

        if (auto&& targetPhysics = registry.try_get<imp::PhysicsBodyComponent>(target.targetEntity)) {
            targetPos = imp::phys::ToGLM(bodyInterface.GetPosition(targetPhysics->id));
        }
        else {
            auto&& targetTransform = registry.try_get<imp::TransformComponent>(target.targetEntity);
            if (!targetTransform) {
                return;
            }
            targetPos = targetTransform->position;
        }
        direction.direction = glm::normalize(targetPos - pos);
        });

};

