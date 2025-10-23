#include "Components/MoveDirectionComponent.h"
#include "Components/PhysicsBodyComponent.h"
#include "Components/SpeedComponent.h"
#include "Components/TransformComponent.h"
#include "CtxRef.h"
#include "Physics/Physics.h"
#include "Systems/MovementSystem.h"
#include <ThreadPool.h>

void MovementSystem::update(entt::registry& registry, const float deltaTime)
{
    auto group = registry.group <MoveDirectionComponent, SpeedComponent>(entt::get<imp::PhysicsBodyComponent>);

    auto&& bodyInterface = registry.ctx().get<CtxRef<imp::phys::Physics>>().get().getSystem().GetBodyInterfaceNoLock();

    group.each([deltaTime, &bodyInterface, &group](auto entity, auto& direction, auto& speed, auto& body) {
        JPH::Vec3 lv, av, p;
        JPH::Quat r;
        bodyInterface.GetPositionAndRotation(body.id, p, r);
        bodyInterface.GetLinearAndAngularVelocity(body.id, lv, av);
        auto d = imp::phys::ToJPH(direction.direction);
        auto tlv = d * speed.maxSpeed;
        auto dlv = tlv - lv;
        auto f = dlv * speed.speed * deltaTime;
        if (direction.faceDirection) {
            JPH::Vec3 forward = r * JPH::Vec3(0.f, 0.f, 1.f);
            auto tr = JPH::Quat::sFromTo(forward, d);
            JPH::Vec3 axis;
            float angle;
            tr.GetAxisAngle(axis, angle);
            auto t = axis * angle * deltaTime;  
            bodyInterface.AddForceAndTorque(body.id, f, t);
        }
        else {
            bodyInterface.AddForce(body.id, f);
        }

        });
}
