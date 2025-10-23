#pragma once
#include "System.h"

namespace imp
{
	// Unused/not implemented
	class RelationshipSystem final : public System
	{
	public:
		virtual void update(entt::registry& registry, const float deltaTime) override;
	};
}
