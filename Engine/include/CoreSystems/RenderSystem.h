#pragma once
#include <entt/entity/observer.hpp>

#include "System.h"

namespace imp {


	class RenderSystem final : public System
	{
	public:
		void initialize(entt::registry& registry) override;
		void update(entt::registry& registry, const float deltaTime) override;
		void cleanup(entt::registry& registry) override;
	private:
		bool m_guiEnabled{ true };

		void OnToggleGuiEvent();
	};
}