#pragma once
#include "System.h"

namespace imp
{
	class RelationshipSystem : public System
	{
	private:

	public:
		virtual void initialize(entt::registry& registry) override;
		virtual void update(entt::registry& registry, const float deltaTime) override;
		virtual void cleanup(entt::registry& registry) override;

		
	};
}
