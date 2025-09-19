#pragma once
#include "Debug.h"
#include "System.h"
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/ContactListener.h>

#include "BodyIDHash.h"
#include "ContactEvent.h"
#include "FreeList.h"
#include "PhysicsCommon.h"

namespace imp::Phys {

	using ContactSignal = entt::sigh<void(const ContactEvent&)>;
	using ContactSink = entt::sink<ContactSignal>;
	using ContactRemovedSignal = entt::sigh<void(std::array<JPH::BodyID, 2>)>;
	using ContactRemovedSink = entt::sink<ContactRemovedSignal>;

	class ContactListener final : public JPH::ContactListener
	{
	private:
		std::unordered_map<JPH::BodyID, entt::entity, BodyIDHash>* bodyToEntityMap;
		std::unordered_map<entt::entity, std::vector<entt::entity>> collidingEntitiesMap;

		entt::dispatcher* dispatcher;
	public:
		ContactListener() = default;
		ContactListener(entt::dispatcher& dispatcher, std::unordered_map<JPH::BodyID, entt::entity, BodyIDHash>& bodyToEntityMap);
		void updateDispatcher(entt::dispatcher& dispatcher);

		// See: ContactListener
		virtual JPH::ValidateResult OnContactValidate(const JPH::Body& inBody1, const JPH::Body& inBody2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult& inCollisionResult) override;

		virtual void OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override;

		virtual void OnContactPersisted(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override;

		virtual void OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair) override;
	};
}
