#pragma once
#include "utils/QuickMacros.h"
#include "utils/VKCommon.hpp"
struct SwapChainSupportDetails
{
	vk::SurfaceCapabilitiesKHR capabilities{};
	std::vector<vk::SurfaceFormatKHR> formats{};
	std::vector<vk::PresentModeKHR> presentModes{};
	SwapChainSupportDetails(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface) :
		capabilities(device.getSurfaceCapabilitiesKHR(surface)),
		formats(device.getSurfaceFormatsKHR(surface)),
		presentModes(device.getSurfacePresentModesKHR(surface))
	{}
	SwapChainSupportDetails() {}
};
