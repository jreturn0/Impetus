#include "MapSystem.h"

#include "Components/ModelComponent.h"
#include "Components/PhysicsBodyComponent.h"
#include "Components/TransformComponent.h"

void MapSystem::initialize(entt::registry& registry)
{
	const int halfWidth = mapWidth / 2, halfLength = mapLength / 2, halfTileSize = tileSize / 2;
	const float tileSizeF = tileSize;

	const auto entity = registry.create();

	registry.emplace<Imp::TransformComponent>(entity, glm::vec3(0, -mapWidth, 0), glm::quat{ 1.f,0.f,0.f,0 }, glm::vec3(mapWidth, mapWidth, mapLength));
	registry.emplace<Imp::ModelComponent>(entity, file.data(), mesh.data(), material.data());
	registry.emplace<Imp::PhysicsBodyComponent>(entity, Imp::Phys::Shape{ Imp::Phys::ShapeType::Box,glm::vec3{mapWidth,mapWidth,mapLength} }, Imp::Phys::MovementType::Static, Imp::Phys::ObjectLayers::LAYER2);


	for (int x = -halfWidth; x < halfWidth; x++) {
		for (int z = -halfLength; z < halfLength; z++) {
			glm::vec3 scale = glm::vec3(tileSize, tileSize, tileSize);
			const auto entityEdge = registry.create();

			if (bool isEdge = x == -halfWidth || x == halfWidth - 1 || z == -halfLength || z == halfLength - 1) {
				scale.y = tileSizeF * mapWidth;
				registry.emplace<Imp::PhysicsBodyComponent>(entityEdge, Imp::Phys::Shape{ Imp::Phys::ShapeType::Box,glm::vec3{tileSizeF,tileSize * mapWidth,tileSizeF} }, Imp::Phys::MovementType::Static, Imp::Phys::ObjectLayers::LAYER2);
			}
			registry.emplace<Imp::TransformComponent>(entityEdge, glm::vec3((x * (2 * tileSize)), -mapWidth, (z * (2 * tileSize))), glm::quat{ 1.f,0.f,0.f,0.f }, scale);
			registry.emplace<Imp::ModelComponent>(entityEdge, file.data(), mesh.data(), material.data());
		}
	}

}

void MapSystem::update(entt::registry& registry, const float deltaTime)
{}

void MapSystem::cleanup(entt::registry& registry)
{}
