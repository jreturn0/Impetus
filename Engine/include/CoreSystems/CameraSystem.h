#pragma once
#include "System.h"

namespace imp
{
	class CameraSystem final :  public System
	{
	public:
		virtual void initialize(entt::registry& registry) override;
		virtual void update(entt::registry& registry, const float deltaTime) override;
	private:
		void OnActiveCameraTagCreate(entt::registry& registry, entt::entity entity);
	};
}
