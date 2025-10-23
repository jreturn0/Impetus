#include "Components/ModelComponent.h"
#include "Components/PhysicsBodyComponent.h"
#include "Components/TransformComponent.h"
#include "Systems/MapSystem.h"

void MapSystem::initialize(entt::registry& registry)
{
	const int halfWidth = mapWidth / 2, halfLength = mapLength / 2, halfTileSize = tileSize / 2;
	const float tileSizeF = static_cast<float>(tileSize);

	const auto entity = registry.create();

	registry.emplace<imp::TransformComponent>(entity, glm::vec3(0, -mapWidth, 0), glm::quat{ 1.f,0.f,0.f,0 }, glm::vec3(mapWidth, mapWidth, mapLength));
	registry.emplace<imp::ModelComponent>(entity, file.data(), mesh.data(), material.data());
	registry.emplace<imp::PhysicsBodyComponent>(entity, imp::phys::Shape{ imp::phys::ShapeType::Box,glm::vec3{mapWidth,mapWidth,mapLength} }, imp::phys::MovementType::Static, imp::phys::ObjectLayers::LAYER2);


	for (int x = -halfWidth; x < halfWidth; x++) {
		for (int z = -halfLength; z < halfLength; z++) {
			glm::vec3 scale = glm::vec3(tileSize, tileSize, tileSize);
			const auto entityEdge = registry.create();

			if (bool isEdge = x == -halfWidth || x == halfWidth - 1 || z == -halfLength || z == halfLength - 1) {
				scale.y = tileSizeF * mapWidth;
				registry.emplace<imp::PhysicsBodyComponent>(entityEdge, imp::phys::Shape{ imp::phys::ShapeType::Box,glm::vec3{tileSizeF,tileSize * mapWidth,tileSizeF} }, imp::phys::MovementType::Static, imp::phys::ObjectLayers::LAYER2);
			}
			registry.emplace<imp::TransformComponent>(entityEdge, glm::vec3((x * (2 * tileSize)), -mapWidth, (z * (2 * tileSize))), glm::quat{ 1.f,0.f,0.f,0.f }, scale);
			registry.emplace<imp::ModelComponent>(entityEdge, file.data(), mesh.data(), material.data());
		}
	}

}

