#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <set>
#pragma warning(push)
#pragma warning(disable: 4996)
#include "vulkan/vulkan.hpp"
#pragma warning(pop)
#include <source_location>
#include "Debug.h"


namespace Imp::Render
{
	enum QueueFamily
	{
		Graphics,
		Present,
		Transfer
	};
}
namespace Imp::Render::vkutil {
	enum class ShaderStage : uint8_t
	{
		None = INT8_MAX,
		Vertex = 0,
		Fragment = 1,
		Compute = 2,
		Geometry = 3
	};

	constexpr std::array VALIDATION_LAYERS = {
		   "VK_LAYER_KHRONOS_validation"
	};
	constexpr std::array INSTANCE_EXTENSIONS = {
		vk::EXTSwapchainColorSpaceExtensionName
	};

	constexpr std::array DEVICE_EXTENSIONS = {
		   vk::KHRSwapchainExtensionName,
	};
#ifdef NDEBUG
	constexpr bool ENABLE_VALIDATION_LAYERS = false;
#else
	constexpr bool ENABLE_VALIDATION_LAYERS = true;
#endif
	constexpr int MAX_FRAMES_IN_FLIGHT = 2;

	using ConstInstRef = const vk::Instance&;
	bool CheckValidationLayerSupport();;
	std::vector<const char*> GetRequiredExtensions();;


	inline bool CheckResult(const vk::Result result, const std::source_location& src = std::source_location::current())
	{
		if (result != vk::Result::eSuccess) {
			std::cerr << "Fatal : VkResult is \"" << vk::to_string(result) << "\" in " << src.file_name()
				<< " at line " << src.line() << std::endl;
			throw std::runtime_error("Vulkan error encountered!");
			return false;
		}
		return true;
	}

	inline bool CheckExtensionsAvailability(const std::vector<const char*>& requiredExtensions)
	{
		const auto&& availableExtensions = vk::enumerateInstanceExtensionProperties();
		std::set<std::string> availableExtensionsSet;
		for (const auto& extension : availableExtensions) {
			availableExtensionsSet.insert(extension.extensionName);
		}
		if (std::ranges::all_of(requiredExtensions, [&availableExtensionsSet](const auto& required) {
			if (availableExtensionsSet.contains(required))
				return true;
			Debug::FatalError("Required extension not available: {}", required);
			return false;
								 })) {
			return true;
		}
		return false;
	}
}
inline bool Imp::Render::vkutil::CheckValidationLayerSupport()
{
	const std::vector<vk::LayerProperties> availableLayers = vk::enumerateInstanceLayerProperties();
	for (const char* layerName : VALIDATION_LAYERS) {
		bool layerFound = false;
		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}
		if (!layerFound) {
			std::cerr << "Validation layer not available: " << layerName << std::endl;
			return false;
		}
	}
	return true;
}

inline std::vector<const char*> Imp::Render::vkutil::GetRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
	if constexpr (ENABLE_VALIDATION_LAYERS) { extensions.push_back(vk::EXTDebugUtilsExtensionName); }
	extensions.insert(extensions.end(), INSTANCE_EXTENSIONS.begin(), INSTANCE_EXTENSIONS.end());
	return extensions;
}
