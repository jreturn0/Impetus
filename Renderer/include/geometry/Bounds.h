#pragma once
#include <glm/vec3.hpp>

namespace imp::gfx {
	// 3D Object Bounds Structure (AABB and Bounding Sphere)
	struct Bounds
	{
		glm::vec3 origin{};
		float sphereRadius{};
		glm::vec3 extents{};
	};
}
