#pragma once
#include <optional>
#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"
namespace Imp::Render {
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;
		std::optional<uint32_t> transferFamily;

		inline bool isComplete() const { return graphicsFamily.has_value() && presentFamily.has_value() && transferFamily.has_value(); }
		
	};

	namespace vkutil
	{
		QueueFamilyIndices FindQueueFamilies(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface);
	}
}
	