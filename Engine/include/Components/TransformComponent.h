#pragma once
#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>


namespace Imp
{
	struct TransformComponent
	{
		glm::vec3 position {0.f};
		glm::quat rotation {1.f,0.f,0.f,0.f};
		glm::vec3 scale {1.f};
		bool dirty = false;


	};

	inline glm::vec3 GetTransformForward(const TransformComponent& transform)
	{
		return glm::rotate(transform.rotation, glm::vec3(0, 0, -1));
	}

	inline glm::vec3 GetTransformRight(const TransformComponent& transform)
	{
		return glm::rotate(transform.rotation, glm::vec3(1, 0, 0));
	}

	inline glm::vec3 GetTransformUp(const TransformComponent& transform)
	{
		return glm::rotate(transform.rotation, glm::vec3(0, 1, 0));
	}

}
