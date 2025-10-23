#include "Components/CollisionInfoComponent.h"
#include "Components/PhysicsBodyComponent.h"
#include "CoreSystems/PhysicsSystem.h"
#include "CtxRef.h"
#include "EngineStats.h"
#include "Physics/Physics.h"
#include "Physics/PhysicsCommon.h"
#include <Components/TransformComponent.h>

namespace {
    JPH::Shape* GetShape(const imp::phys::ShapeType type, const glm::vec3& shapeData)
    {
        switch (type) {
        case imp::phys::ShapeType::None:return new JPH::BoxShape({ 0.5f,0.5f,0.5f });
        case imp::phys::ShapeType::Box: return new JPH::BoxShape(imp::phys::ToJPH((shapeData)));
        case imp::phys::ShapeType::Sphere: return new JPH::SphereShape((shapeData.x));
        case imp::phys::ShapeType::Capsule: return new JPH::CapsuleShape(shapeData.x, shapeData.y);
        case imp::phys::ShapeType::Cylinder: return new JPH::CylinderShape(shapeData.x, shapeData.y);
        default: return new JPH::BoxShape({ 0.5f,0.5f,0.5f });
        }
    }
}

void imp::PhysicsSystem::OnPhysicsBodyComponentConstruct(entt::registry& registry, entt::entity entity)
{
    const auto& transform = registry.try_get<TransformComponent>(entity);
    const auto position = transform != nullptr ? transform->position : glm::vec3{ 0.f,0.f,0.f };
    const auto rotation = transform != nullptr ? transform->rotation : glm::quat{ 1.f,0.f,0.f,0.f };

    auto& body = registry.get<PhysicsBodyComponent>(entity);

    JPH::Shape* shape = GetShape(body.shape.type, body.shape.data);

    const auto id = m_physicsEngine->createBody(entity, shape, position, rotation, static_cast<JPH::EMotionType>(body.movementType), static_cast<JPH::ObjectLayer>(body.layer));
    if (id.IsInvalid()) {
        Debug::FatalError("Physics Body ID is invalid");
    }
    else
        Debug::Info("New Physics Body ID: {}", id.GetIndex());
    auto&& bodyInterface = m_physicsEngine->getBodyInterface();
    auto castedShapeType = static_cast<JPH::uint64>(body.shape.type);
    auto movementType = static_cast<JPH::EMotionType>(body.movementType);

    bodyInterface.SetMotionType(body.id, movementType, JPH::EActivation::DontActivate);
    bodyInterface.SetUserData(id, castedShapeType);
    bodyInterface.SetLinearAndAngularVelocity(id, phys::ToJPH(body.linearVelocity), phys::ToJPH(body.angularVelocity));
    bodyInterface.SetFriction(id, body.friction);
    bodyInterface.SetRestitution(id, body.restitution);
    body.id = id;

    auto shape_ = phys::ToString(body.shape.type);
    auto movement_ = phys::ToString(body.movementType);

    Debug::Info("Entity {} Constructed Physics Body {} with:\n\tShape: {}\n\tMovement: {}", entt::to_integral(entity), id.GetIndexAndSequenceNumber(), shape_.data(), movement_.data());
}

void imp::PhysicsSystem::OnPhysicsBodyComponentDestroy(entt::registry& registry, entt::entity entity) const
{

    auto& bodyComponent = registry.get<PhysicsBodyComponent>(entity);
    if (!bodyComponent.id.IsInvalid()) {
        m_physicsEngine->destroyBody(bodyComponent.id);
    }
}

void imp::PhysicsSystem::OnPhysicsBodyComponentUpdate(entt::registry& registry, entt::entity entity)
{
    auto& body = registry.get<PhysicsBodyComponent>(entity);

    if (body.id.IsInvalid()) {
        OnPhysicsBodyComponentConstruct(registry, entity);
        return;
    }
    const auto&& transform = registry.try_get<TransformComponent>(entity);
    const auto position = transform != nullptr ? (transform->position) : glm::vec3{ 0.f };
    const auto rotation = transform != nullptr ? (transform->rotation) : glm::quat{ 1.f,0.f,0.f,0.f };

    auto&& bodyInterface = m_physicsEngine->getBodyInterface();
    auto&& userData = bodyInterface.GetUserData(body.id);

    // Cast userData to the appropriate type
    const auto currentShapeType = static_cast<JPH::uint64>(userData);

    // Ensure shape type is compared correctly
    if (const auto shape = static_cast<JPH::uint64>(body.shape.type); currentShapeType != shape || body.forceUpdateShape) {
        bodyInterface.SetUserData(body.id, shape);
        bodyInterface.SetShape(body.id, GetShape(body.shape.type, body.shape.data), true, JPH::EActivation::Activate);
        body.forceUpdateShape = false;
    }
    const auto currentMotionType = bodyInterface.GetMotionType(body.id);

    // Ensure motion type is compared correctly
    if (const auto expectedMotionType = static_cast<JPH::EMotionType>(body.movementType); currentMotionType != expectedMotionType) {
        bodyInterface.SetMotionType(body.id, expectedMotionType, JPH::EActivation::DontActivate);
    }
    bodyInterface.SetPosition(body.id, phys::ToJPH(position), JPH::EActivation::DontActivate);
    bodyInterface.SetRotation(body.id, phys::ToJPH(rotation), JPH::EActivation::DontActivate);
    bodyInterface.SetLinearAndAngularVelocity(body.id, phys::ToJPH(body.linearVelocity), phys::ToJPH(body.angularVelocity));
    bodyInterface.SetFriction(body.id, body.friction);
    bodyInterface.SetRestitution(body.id, body.restitution);
}

void imp::PhysicsSystem::OnCollisionInfoConstruct(entt::registry& registry, entt::entity entity) const
{
    registry.get<CollisionInfoComponent>(entity).collidingEntities.clear();
}

void imp::PhysicsSystem::handleCollision(entt::registry& registry, const imp::phys::ContactEvent& event)
{
    auto updateCollisionComponent = [&](entt::entity entity, entt::entity otherEntity) {
        if (registry.all_of<CollisionInfoComponent>(entity)) {
            auto& collisionComponent = registry.get<CollisionInfoComponent>(entity);
            if (event.type != phys::ContactEventType::Removed) {
                collisionComponent.collidingEntities.insert(otherEntity);
            }
            else {
                collisionComponent.collidingEntities.erase(otherEntity);
            }
        }
        };

    if (registry.valid(event.entity) && registry.valid(event.otherEntity)) {
        updateCollisionComponent(event.entity, event.otherEntity);
        updateCollisionComponent(event.otherEntity, event.entity);
    }
}


void imp::PhysicsSystem::OnCollisionEvent(const imp::phys::ContactEvent& event)
{
    std::lock_guard<std::mutex> lock(m_collisionEventsMutex);
    m_collisionEvents.emplace(event);
}




void imp::PhysicsSystem::initialize(entt::registry& registry)
{
    m_physicsEngine = &registry.ctx().get<CtxRef<phys::Physics>>().get();
    auto&& group = registry.view<PhysicsBodyComponent, TransformComponent>();

    for (auto&& entity : group) {
        OnPhysicsBodyComponentConstruct(registry, entity);
    }

    registry.on_construct<PhysicsBodyComponent>().connect<&PhysicsSystem::OnPhysicsBodyComponentConstruct>(*this);
    registry.on_destroy<PhysicsBodyComponent>().connect<&PhysicsSystem::OnPhysicsBodyComponentDestroy>(*this);
    registry.on_update<PhysicsBodyComponent>().connect<&PhysicsSystem::OnPhysicsBodyComponentUpdate>(*this);
    registry.on_construct<CollisionInfoComponent>().connect<&PhysicsSystem::OnCollisionInfoConstruct>(*this);

    registry.ctx().get<CtxRef<entt::dispatcher>>().get().sink<phys::ContactEvent>().connect<&PhysicsSystem::OnCollisionEvent>(*this);
}



void imp::PhysicsSystem::update(entt::registry& registry, const float deltaTime)
{
    using namespace utl::hashLiterals;
    auto& stats = registry.ctx().get<CtxRef<EngineStats>>().get();

    ENGINESTATS_SCOPED_TIMER(stats, "physicsUpdate")
    {
        ENGINESTATS_SCOPED_TIMER(stats, "handleCollision")
            while (!m_collisionEvents.empty()) {
                handleCollision(registry, m_collisionEvents.front());
                m_collisionEvents.pop();
            }
    }
    while (!m_pendingEvents.empty()) {
        m_pendingEvents.front()();
        m_pendingEvents.pop();
    }
    m_physicsEngine->update(deltaTime);
}

void imp::PhysicsSystem::cleanup(entt::registry& registry)
{


}
