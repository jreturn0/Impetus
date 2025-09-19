#pragma once
#include <glm/mat4x4.hpp>
#include "utils/VKCommon.hpp"
namespace imp::gfx
{
	struct GPUDrawPushConstants
	{
		glm::mat4 worldMatrix;
		vk::DeviceAddress vertexBuffer;
	};
}
