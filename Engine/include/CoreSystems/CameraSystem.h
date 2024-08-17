#pragma once
#include "System.h"

namespace Imp
{
	class CameraSystem : public System
	{
	private:
		void OnActiveCameraTagCreate(entt::registry& registry, entt::entity entity);
	public:
		virtual void initialize(entt::registry& registry) override;
		virtual void update(entt::registry& registry, const float deltaTime) override;
		virtual void cleanup(entt::registry& registry) override;

		
	};
}
