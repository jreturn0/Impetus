#pragma once
#include "System.h"


	class MapSystem : public System
	{
	private:

		const int mapWidth = 15, mapLength = 15, tileSize = 20;
		std::string_view file{ "basic_shapes.glb" }, mesh{ "Cube" }, material{ "Sand" };
	public:
		virtual void initialize(entt::registry& registry) override;
		virtual void update(entt::registry& registry, const float deltaTime) override;
		virtual void cleanup(entt::registry& registry) override;

		
	};

