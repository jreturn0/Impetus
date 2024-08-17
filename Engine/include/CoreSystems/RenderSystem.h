#pragma once
#include <entt/entity/observer.hpp>

#include "System.h"

namespace Imp {


	class RenderSystem final : public System
	{
	private:
		//entt::observer observer;
		bool gui = true;
	public:
		void OnToggleGuiEvent();
		void initialize(entt::registry& registry) override;
		void update(entt::registry& registry, const float deltaTime) override;
		void cleanup(entt::registry& registry) override;
	};
}