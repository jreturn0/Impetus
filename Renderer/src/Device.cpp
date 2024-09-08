#include "pch.h"
#include "core/Device.h"

#include <set>

#include "core/SwapChainSupportDetails.h"
#include "window/VKWindow.h"
#include "core/Instance.h"

namespace
{
	bool CheckDeviceExtensionSupport(const vk::PhysicalDevice& device)
	{
		auto extProps = device.enumerateDeviceExtensionProperties();
		std::set<std::string> requiredExtensions(Imp::Render::vkutil::DEVICE_EXTENSIONS.begin(),
		                                         Imp::Render::vkutil::DEVICE_EXTENSIONS.end());

		for (const auto& extension : extProps)
		{
			//	std::cout << "Available extension: " << extension.extensionName << std::endl;
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	bool IsDeviceSuitable(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface)
	{
		Imp::Render::QueueFamilyIndices indices = Imp::Render::vkutil::FindQueueFamilies(device, surface);

		bool extensionsSupported = CheckDeviceExtensionSupport(device);
		SwapChainSupportDetails swapChainSupport = {device, surface};
		bool swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		vk::PhysicalDeviceFeatures supportedFeatures = device.getFeatures();

		return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.
			samplerAnisotropy;
	}


	vk::PhysicalDevice PickPhysicalDevice(const vk::Instance& instance, const vk::SurfaceKHR& surface)
	{
		auto devices = instance.enumeratePhysicalDevices();
		if (devices.empty()) { throw std::runtime_error("Failed to find GPUs with Vulkan support!"); }

		for (const auto& device : devices) { if (IsDeviceSuitable(device, surface)) { return device; } }

		throw std::runtime_error("Failed to find a suitable GPU!");
	}


	vk::UniqueDevice CreateDevice(const vk::PhysicalDevice& physicalDevice,
	                              const Imp::Render::QueueFamilyIndices& indices)
	{
		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = {
			indices.graphicsFamily.value(), indices.presentFamily.value(), indices.transferFamily.value()
		};
		float queuePriority = 1.0f;
		queueCreateInfos.reserve(uniqueQueueFamilies.size());
		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			queueCreateInfos.emplace_back(vk::DeviceQueueCreateInfo({}, queueFamily, 1, &queuePriority));
		}
		vk::PhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddressFeatures(true);
		vk::PhysicalDeviceSynchronization2FeaturesKHR synchronization2Features(true, &bufferDeviceAddressFeatures);
		vk::PhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeatures(true, &synchronization2Features);

		vk::PhysicalDeviceFeatures2 deviceFeatures2({}, &dynamicRenderingFeatures);


		vk::PhysicalDeviceFeatures deviceFeatures{};
		auto layers = std::vector<const char*>();
		if constexpr (Imp::Render::vkutil::ENABLE_VALIDATION_LAYERS)
			layers.insert(layers.end(), Imp::Render::vkutil::VALIDATION_LAYERS.begin(),
			              Imp::Render::vkutil::VALIDATION_LAYERS.end());


		vk::DeviceCreateInfo deviceCreateInfo({}, queueCreateInfos, layers, Imp::Render::vkutil::DEVICE_EXTENSIONS, {},
		                                      &deviceFeatures2);
		return physicalDevice.createDeviceUnique(deviceCreateInfo);
	}
}

Imp::Render::Device::Device(const vk::Instance& instance, const vk::SurfaceKHR& surface) :
	physical(PickPhysicalDevice(instance, surface)),
	indices(vkutil::FindQueueFamilies(physical, surface)),
	logical((CreateDevice(physical, indices)))
{
}

Imp::Render::Device::~Device() = default;

Imp::Render::UniqueDevice Imp::Render::CreateUniqueDevice(const Instance& instance, const VKWindow& surface)
{
	return std::make_unique<Device>(instance, surface.getSurface());
}
