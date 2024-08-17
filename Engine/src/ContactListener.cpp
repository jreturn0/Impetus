#include "Physics/ContactListener.h"
#include<Jolt/Physics/Constraints/ContactConstraintManager.h>








Imp::Phys::ContactListener::ContactListener(entt::dispatcher& dispatcher,
											std::unordered_map<JPH::BodyID, entt::entity, BodyIDHash>& bodyToEntityMap) : bodyToEntityMap(&bodyToEntityMap),
	dispatcher(&dispatcher)
{}

void Imp::Phys::ContactListener::updateDispatcher(entt::dispatcher& dispatcher)
{
	this->dispatcher = &dispatcher;
}

JPH::ValidateResult Imp::Phys::ContactListener::OnContactValidate(const JPH::Body& inBody1, const JPH::Body& inBody2,
																  JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult& inCollisionResult)
{
	//	Debug::Info("Contact validate callback");
		// Allows you to ignore a contact before it is created (using layers to not make objects collide is cheaper!)
	return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
}

void Imp::Phys::ContactListener::OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2,
												const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings)
{
	dispatcher->trigger(
		ContactEvent{(*bodyToEntityMap)[inBody1.GetID()],(*bodyToEntityMap)[inBody2.GetID()],ContactEventType::Added});
}

void Imp::Phys::ContactListener::OnContactPersisted(const JPH::Body& inBody1, const JPH::Body& inBody2,
													const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings)
{
	dispatcher->trigger(
		ContactEvent{(*bodyToEntityMap)[inBody1.GetID()],(*bodyToEntityMap)[inBody2.GetID()],ContactEventType::Persist});
}

void Imp::Phys::ContactListener::OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair)
{
	//	(*bodyToEntityMap)[ inSubShapePair.GetBody1ID()]; // [inSubShapePair.GetBody1ID()]
	dispatcher->trigger(
		ContactEvent{(*bodyToEntityMap)[inSubShapePair.GetBody1ID()],(*bodyToEntityMap)[inSubShapePair.GetBody2ID()],ContactEventType::Removed});
}
