#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "QuickMacros.h"
#include <fmt\core.h>
#include <set>
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#pragma warning(push)
#pragma warning(disable: 4996)
#include "vulkan/vulkan.hpp"
#pragma warning(pop)
#include <source_location>
#include "VkConstants.h"



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
		vk::EXTSwapchainColorSpaceExtensionName,
		vk::EXTDebugUtilsExtensionName
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


	using ConstInstRef = const vk::Instance&;
	bool CheckValidationLayerSupport();;
	std::vector<const char*> GetRequiredExtensions();;


	inline void CheckResult(const vk::Result result, const std::source_location& src = std::source_location::current())
	{
		if (result != vk::Result::eSuccess) {

			fmt::print(stderr, "Fatal: VkResult is \"{}\" in {} at line {}\n",
					   vk::to_string(result), src.file_name(), src.line());
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
#ifndef NDEBUG
			fmt::println(stderr, "Required extension: {}", required);
#endif
			//std::cout << "Required extension: " << required << std::endl;
			if (!availableExtensionsSet.contains(required)) {
#ifndef NDEBUG
				fmt::println(stderr, "Required extension not available: {}", required);
#endif
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
			fmt::print(stderr, "Validation layer not available: {}\n", layerName);
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
