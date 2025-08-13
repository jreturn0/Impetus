#pragma once
#include <glm/vec3.hpp>

namespace Imp::Render {
	struct Bounds
	{
		glm::vec3 origin;
		float sphereRadius;
		glm::vec3 extents;
	};
}
