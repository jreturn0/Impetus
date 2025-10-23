#pragma once
#include "System.h"
namespace imp {
	class EndRenderSystem final : public  System
	{
	public:
		void initialize(entt::registry& registry) override;
		void update(entt::registry& registry, const float deltaTime) override;
	private:
		bool m_guiEnabled = true;
		void OnToggleGuiEvent();
	};
}