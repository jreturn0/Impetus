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




namespace Imp::Render::vkutil {
	enum class ShaderStage
	{
		None = INT32_MAX,
		Vertex = 0,
		Fragment = 1,
		Compute = 2,
		Geometry = 3
	};
	const std::vector<const char*> VALIDATION_LAYERS = {
		   "VK_LAYER_KHRONOS_validation"
	};
	const std::vector INSTANCE_EXTENSIONS = {
		vk::EXTSwapchainColorSpaceExtensionName
	};

	const std::vector<const char*> DEVICE_EXTENSIONS = {
		   vk::KHRSwapchainExtensionName,
		VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
		VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
		VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME

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


	inline void CheckResult(const vk::Result result, const std::source_location& src = std::source_location::current())
	{
		if (result != vk::Result::eSuccess) {
			std::cerr << "Fatal : VkResult is \"" << vk::to_string(result) << "\" in " << src.file_name()
				<< " at line " << src.line() << std::endl;
			throw std::runtime_error("Vulkan error encountered!");
		}
	}

	inline bool CheckExtensionsAvailability(const std::vector<const char*>& requiredExtensions)
	{
		auto availableExtensions = vk::enumerateInstanceExtensionProperties();
		std::set<std::string> availableExtensionsSet;
		for (const auto& extension : availableExtensions) {
			availableExtensionsSet.insert(extension.extensionName);
		}

		for (const auto& required : requiredExtensions) {
			//std::cout << "Required extension: " << required << std::endl;
			if (!availableExtensionsSet.contains(required)) {
			//	std::cerr << "Required extension not available: " << required << std::endl;
				return false;
			}
		}
		return true;
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
	if (ENABLE_VALIDATION_LAYERS) { extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME); }
	extensions.insert(extensions.end(), INSTANCE_EXTENSIONS.begin(), INSTANCE_EXTENSIONS.end());
	return extensions;
}
