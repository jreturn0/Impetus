#pragma once
#include <string>
#include <glm/mat4x4.hpp>
namespace imp
{
	struct ModelComponent
	{
		std::string gltf;
		std::string name;
		std::string materialOverrideName;
		glm::mat4 localTransform {1.f};
		bool visible = true;
	};

}
