#include "CoreSystems/RelationshipSystem.h"

#include "Components/RelationshipComponent.h"
#include "Components/TransformComponent.h"

void Imp::RelationshipSystem::initialize(entt::registry& registry)
{}

void Imp::RelationshipSystem::update(entt::registry& registry, const float deltaTime)
{
	auto&& group = registry.group<Imp::RelationshipComponent>(entt::get<TransformComponent>);
	group.sort([&registry](const entt::entity lhs, const entt::entity rhs) {
		//const auto& lhsRel = registry.get<Imp::RelationshipComponent>(lhs);
		const auto& rhsRel = registry.get<Imp::RelationshipComponent>(rhs);

		// Check if lhs is an ancestor of rhs
		entt::entity current = rhsRel.parent;
		while (registry.valid(current)) {
			if (current == lhs) {
				return true;  // lhs should come before rhs
			}
			if (auto&& rel = registry.try_get<Imp::RelationshipComponent>(current))
				current = rel->parent;
			else
				return false;

		}

		return false;
			   });

	group.each([&registry](auto&& entity, auto&& relationship, auto&& transform) {
		if (registry.valid(relationship.parent)) {
			// Retrieve the parent's transform

			auto& parentTransform = registry.get<TransformComponent>(relationship.parent);
			glm::mat4 parentMatrix = glm::translate(glm::mat4(1.0f), parentTransform.position)
				* glm::mat4_cast(parentTransform.rotation)
				* glm::scale(glm::mat4(1.0f), parentTransform.scale);

			glm::mat4 childMatrix = glm::translate(glm::mat4(1.0f), transform.position)
				* glm::mat4_cast(transform.rotation)
				* glm::scale(glm::mat4(1.0f), transform.scale);

			glm::mat4 combinedMatrix = parentMatrix * childMatrix;

			// Extract the final world transform from the combined matrix
			transform.position = glm::vec3(combinedMatrix[3]);
			transform.rotation = glm::quat_cast(combinedMatrix);
			transform.scale = glm::vec3(combinedMatrix[0][0], combinedMatrix[1][1], combinedMatrix[2][2]);
		}
			   });



}

void Imp::RelationshipSystem::cleanup(entt::registry& registry)
{}
