#include "PhysicsSystem.h"

#include "CtxRef.h"
#include "Physics/Physics.h"
#include "Physics/PhysicsCommon.h"
#include "Components/PhysicsBodyComponent.h"
#include <Components/TransformComponent.h>

#include "EngineStats.h"
#include "Components/CollisionInfoComponent.h"


JPH::Shape* GetShape(const Imp::Phys::ShapeType type, const glm::vec3& shapeData)
{
	switch (type) {
	case Imp::Phys::ShapeType::None:
		return new JPH::BoxShape({ 0.5f,0.5f,0.5f });

	case Imp::Phys::ShapeType::Box:
		return new JPH::BoxShape(Imp::Phys::ToJPH((shapeData)));
	case Imp::Phys::ShapeType::Sphere:
		return new JPH::SphereShape((shapeData.x));
	case Imp::Phys::ShapeType::Capsule:
		return new JPH::CapsuleShape(shapeData.x, shapeData.y);
	case Imp::Phys::ShapeType::Cylinder:
		return new JPH::CylinderShape(shapeData.x, shapeData.y);
	default:
		return new JPH::BoxShape({ 0.5f,0.5f,0.5f });
	}
}

void Imp::PhysicsSystem::OnPhysicsBodyComponentConstruct(entt::registry& registry, entt::entity entity)
{


	const auto& transform = registry.try_get<TransformComponent>(entity);

	const auto position = transform != nullptr ? transform->position : glm::vec3{0.f,0.f,0.f};
	const auto rotation = transform != nullptr ? transform->rotation : glm::quat{1.f,0.f,0.f,0.f};



	auto& body = registry.get<PhysicsBodyComponent>(entity);



	JPH::Shape* shape = GetShape(body.shape.type, body.shape.data);




	const auto id = physicsSystem->createBody(entity, shape, position, rotation, static_cast<JPH::EMotionType>(body.movementType), static_cast<JPH::ObjectLayer>(body.layer));
	if (id.IsInvalid()) {
		Debug::FatalError("Physics Body ID is invalid");
	} else
		Debug::Info("New Physics Body ID: {}", id.GetIndex());
	auto&& bodyInterface = physicsSystem->getBodyInterface();
	auto castedShapeType = static_cast<JPH::uint64>(body.shape.type);
	auto movementType = static_cast<JPH::EMotionType>(body.movementType);

	bodyInterface.SetMotionType(body.id, movementType, JPH::EActivation::DontActivate);
	bodyInterface.SetUserData(id, castedShapeType);
	bodyInterface.SetLinearAndAngularVelocity(id, Phys::ToJPH(body.linearVelocity), Phys::ToJPH(body.angularVelocity));
	bodyInterface.SetFriction(id, body.friction);
	bodyInterface.SetRestitution(id, body.restitution);
	body.id = id;

	auto shape_ = Phys::ToString(body.shape.type);
	auto movement_ = Phys::ToString(body.movementType);

	Debug::Info("Entity {} Constructed Physics Body {} with:\n\tShape: {}\n\tMovement: {}",entt::to_integral(entity),id.GetIndexAndSequenceNumber(), shape_.data(), movement_.data());

}

void Imp::PhysicsSystem::OnPhysicsBodyComponentDestroy(entt::registry& registry, entt::entity entity) const
{

	auto& bodyComponent = registry.get<PhysicsBodyComponent>(entity);
	if (!bodyComponent.id.IsInvalid()) {
		physicsSystem->destroyBody(bodyComponent.id);
	}
}

void Imp::PhysicsSystem::OnPhysicsBodyComponentUpdate(entt::registry& registry, entt::entity entity)
{
	auto& body = registry.get<PhysicsBodyComponent>(entity);

	if (body.id.IsInvalid()) {
		//Debug::Out("New Physics Body ID: {}", body.id.GetIndex());

		OnPhysicsBodyComponentConstruct(registry, entity);
		return;
	}


	//Debug::Info("Updating Physics Body ID: {}", body.id.GetIndex());

	const auto&& transform = registry.try_get<TransformComponent>(entity);

	const auto position = transform != nullptr ? (transform->position) : glm::vec3{ 0.f };
	const auto rotation = transform != nullptr ? (transform->rotation) : glm::quat{ 1.f,0.f,0.f,0.f };

	auto&& bodyInterface = physicsSystem->getBodyInterface();

	auto&& userData = bodyInterface.GetUserData(body.id);

	// Cast userData to the appropriate type
	const auto currentShapeType = static_cast<JPH::uint64>(userData);

	// Ensure shape type is compared correctly
	if (const auto shape = static_cast<JPH::uint64>(body.shape.type); currentShapeType != shape || body.forceUpdateShape) {
		//Debug::Info("Updating Shape for Physics Body ID: {} To {}", body.id.GetIndex(), Phys::ToString(body.shape.type));
		bodyInterface.SetUserData(body.id, shape);
		bodyInterface.SetShape(body.id, GetShape(body.shape.type, body.shape.data), true, JPH::EActivation::Activate);
		body.forceUpdateShape = false;
	}

	//Debug::Info("Current Shape: c{} | u{}", Phys::ToString(body.shape.type), Phys::ToString(static_cast<Phys::ShapeType>(currentShapeType)));

	const auto currentMotionType = bodyInterface.GetMotionType(body.id);

	// Ensure motion type is compared correctly
	if (const auto expectedMotionType = static_cast<JPH::EMotionType>(body.movementType); currentMotionType != expectedMotionType) {
		//Debug::Info("Updating Movement Type for Physics Body ID: {} To {}", body.id.GetIndex(), Phys::ToString(body.movementType));
		bodyInterface.SetMotionType(body.id, expectedMotionType, JPH::EActivation::DontActivate);
	}

	//Debug::Info("Movement Type: c{} | b{}", Phys::ToString(body.movementType), Phys::ToString(static_cast<Phys::MovementType>(currentMotionType)));

	bodyInterface.SetPosition(body.id, Phys::ToJPH(position), JPH::EActivation::DontActivate);
	bodyInterface.SetRotation(body.id, Phys::ToJPH(rotation), JPH::EActivation::DontActivate);
	bodyInterface.SetLinearAndAngularVelocity(body.id, Phys::ToJPH(body.linearVelocity), Phys::ToJPH(body.angularVelocity));
	bodyInterface.SetFriction(body.id, body.friction);
	bodyInterface.SetRestitution(body.id, body.restitution);
}

void Imp::PhysicsSystem::OnCollisionInfoConstruct(entt::registry& registry, entt::entity entity) const
{
	registry.get<CollisionInfoComponent>(entity).collidingEntities.clear();
}

void Imp::PhysicsSystem::handleCollision(entt::registry& registry, const Imp::Phys::ContactEvent& event)
{
	auto updateCollisionComponent = [&](entt::entity entity, entt::entity otherEntity) {
		if (registry.all_of<CollisionInfoComponent>(entity)) {
			auto& collisionComponent = registry.get<CollisionInfoComponent>(entity);
			if (event.type != Phys::ContactEventType::Removed) {
				collisionComponent.collidingEntities.insert(otherEntity);
			} else {
				collisionComponent.collidingEntities.erase(otherEntity);
			}
		}
		};

	if (registry.valid(event.entity) && registry.valid(event.otherEntity)) {
		updateCollisionComponent(event.entity, event.otherEntity);
		updateCollisionComponent(event.otherEntity, event.entity);
	}
}


void Imp::PhysicsSystem::OnCollisionEvent(const Imp::Phys::ContactEvent& event)
{
	std::lock_guard<std::mutex> lock(collisionEventsMutex);
	collisionEvents.emplace(event);
}




void Imp::PhysicsSystem::initialize(entt::registry& registry)
{
	physicsSystem = &registry.ctx().get<CtxRef<Phys::Physics>>().get();
	auto&& group = registry.view<PhysicsBodyComponent, TransformComponent>();

	for (auto&& entity : group) {
		OnPhysicsBodyComponentConstruct(registry, entity);
	}


	registry_ = &registry;

	registry.on_construct<PhysicsBodyComponent>().connect<&PhysicsSystem::OnPhysicsBodyComponentConstruct>(*this);
	registry.on_destroy<PhysicsBodyComponent>().connect<&PhysicsSystem::OnPhysicsBodyComponentDestroy>(*this);
	registry.on_update<PhysicsBodyComponent>().connect<&PhysicsSystem::OnPhysicsBodyComponentUpdate>(*this);
	registry.on_construct<CollisionInfoComponent>().connect<&PhysicsSystem::OnCollisionInfoConstruct>(*this);

	registry.ctx().get<CtxRef<entt::dispatcher>>().get().sink<Phys::ContactEvent>().connect<&PhysicsSystem::OnCollisionEvent>(*this);
}



void Imp::PhysicsSystem::update(entt::registry& registry, const float deltaTime)
{
	auto& stats = registry.ctx().get<CtxRef<EngineStats>>().get();

	RECORD_STATS_VARIABLES

	RECORD_STATS_START
	while (!collisionEvents.empty()) {
		handleCollision(registry, collisionEvents.front());
		collisionEvents.pop();
	}
	RECORD_SYSTEM_STATS_END_P(stats, "handleCollision")

	while (!deferredEvents.empty()) {
		deferredEvents.front()();
		deferredEvents.pop();
	}
	RECORD_STATS_START
	physicsSystem->update(deltaTime);
	RECORD_SYSTEM_STATS_END_P(stats, "physicsUpdate")

}

void Imp::PhysicsSystem::cleanup(entt::registry& registry)
{


}
