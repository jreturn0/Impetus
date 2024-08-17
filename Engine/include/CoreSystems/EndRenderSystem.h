#pragma once
#include "System.h"
namespace Imp {
	class EndRenderSystem final : public  System
	{
		bool gui = true;
	public:
		void OnToggleGuiEvent();
		void initialize(entt::registry& registry) override;
		void update(entt::registry& registry, const float deltaTime) override;
		void cleanup(entt::registry& registry) override;
	};
}