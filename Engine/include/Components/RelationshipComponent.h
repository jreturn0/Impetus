#pragma once
#include <vector>
#include <entt/entity/entity.hpp>
#include <glm/mat4x4.hpp>
namespace imp
{
	struct RelationshipComponent
	{
		entt::entity parent = entt::null;
		glm::mat4 localTransform = glm::mat4(1.0f);
		//std::vector<entt::entity> children;


	};
}
