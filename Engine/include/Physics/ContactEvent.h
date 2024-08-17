#pragma once
#include <entt/entity/entity.hpp>
#include <glm/vec3.hpp>

#include "PhysicsCommon.h"

namespace Imp::Phys
{
	enum class ContactEventType : uint8_t
	{
		Added,
		Persist,
		Removed
	};	
	struct ContactEvent
	{
		entt::entity entity;
		entt::entity otherEntity;
		//glm::vec3 point;
		//glm::vec3 otherPoint;//same as point if no penetration
		//glm::vec3 normal;
		//float penetrationDepth;
		ContactEventType type;
		//ObjectLayer layer;
	};

}
