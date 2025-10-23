#include "Physics/ContactListener.h"
#include<Jolt/Physics/Constraints/ContactConstraintManager.h>








imp::phys::ContactListener::ContactListener(entt::dispatcher& dispatcher,
    std::unordered_map<JPH::BodyID, entt::entity, BodyIDHash>& bodyToEntityMap) : m_bodyToEntityMap(&bodyToEntityMap),
    m_dispatcher(&dispatcher)
{
}

void imp::phys::ContactListener::updateDispatcher(entt::dispatcher& dispatcher)
{
    this->m_dispatcher = &dispatcher;
}

JPH::ValidateResult imp::phys::ContactListener::OnContactValidate(const JPH::Body& inBody1, const JPH::Body& inBody2,
    JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult& inCollisionResult)
{
    // Allows you to ignore a contact before it is created (using layers to not make objects collide is cheaper!)
    return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
}

void imp::phys::ContactListener::OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2,
    const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings)
{
    m_dispatcher->trigger(
        ContactEvent{ (*m_bodyToEntityMap)[inBody1.GetID()],(*m_bodyToEntityMap)[inBody2.GetID()],ContactEventType::Added });
}

void imp::phys::ContactListener::OnContactPersisted(const JPH::Body& inBody1, const JPH::Body& inBody2,
    const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings)
{
    m_dispatcher->trigger(
        ContactEvent{ (*m_bodyToEntityMap)[inBody1.GetID()],(*m_bodyToEntityMap)[inBody2.GetID()],ContactEventType::Persist });
}

void imp::phys::ContactListener::OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair)
{
    m_dispatcher->trigger(
        ContactEvent{ (*m_bodyToEntityMap)[inSubShapePair.GetBody1ID()],(*m_bodyToEntityMap)[inSubShapePair.GetBody2ID()],ContactEventType::Removed });
}
