#pragma once
#include <entt/entity/entity.hpp>
#include <glm/vec3.hpp>
#include "PhysicsCommon.h"

namespace imp::phys
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
		ContactEventType type;
	};

}
