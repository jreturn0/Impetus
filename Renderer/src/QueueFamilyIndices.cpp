#include "core/QueueFamilyIndices.h"

Imp::Render::QueueFamilyIndices Imp::Render::vkutil::FindQueueFamilies(const vk::PhysicalDevice& device,
	const vk::SurfaceKHR& surface)
{
	QueueFamilyIndices indices;

	auto queueFamilies = device.getQueueFamilyProperties();

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) { indices.graphicsFamily = i; }
		else if (queueFamily.queueFlags & vk::QueueFlagBits::eTransfer) { indices.transferFamily = i; }
		VkBool32 presentSupport = device.getSurfaceSupportKHR(i, surface);
		if (presentSupport) { indices.presentFamily = i; }
		

		if (indices.isComplete()) { break; }

		i++;
	}

	return indices;
}
