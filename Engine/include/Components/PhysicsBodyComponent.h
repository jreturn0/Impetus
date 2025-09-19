#pragma once
#include <glm/vec3.hpp>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>


#include "Physics/PhysicsCommon.h"

namespace imp
{
	//Component representation of a Jolt physics body. use patch or replace to update the Component 
	struct PhysicsBodyComponent
	{
		Phys::Shape shape{};
		Phys::MovementType movementType{Phys::MovementType::Dynamic};
		Phys::ObjectLayer layer{Phys::ObjectLayers::DEFAULT};
		glm::vec3 linearVelocity{0.f};
		glm::vec3 angularVelocity{0.f};
		float friction = 0.5f;
		float restitution = 0.5f;
		JPH::BodyID id = JPH::BodyID{ JPH::BodyID::cInvalidBodyID };
		bool forceUpdateShape = false;

	};
}
