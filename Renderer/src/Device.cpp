//#include "core/Device.h"
//
//#include <set>
//
//#include "core/SwapChainSupportDetails.h"
//#include "window/VKWindow.h"
//#include "core/Instance.h"
//
//bool Imp::Render::Device::IsDeviceSuitable(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface)
//{
//	QueueFamilyIndices indices = vkutil::FindQueueFamilies(device, surface);
//
//	bool extensionsSupported = CheckDeviceExtensionSupport(device);
//	SwapChainSupportDetails swapChainSupport = { device, surface };
//	bool swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
//	vk::PhysicalDeviceFeatures supportedFeatures = device.getFeatures();
//
//	return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.
//		samplerAnisotropy;
//}
//
//bool Imp::Render::Device::CheckDeviceExtensionSupport(const vk::PhysicalDevice& device)
//{
//	auto extProps = device.enumerateDeviceExtensionProperties();
//	std::set<std::string> requiredExtensions(vkutil::DEVICE_EXTENSIONS.begin(), vkutil::DEVICE_EXTENSIONS.end());
//
//	for (const auto& extension : extProps) {
//	//	std::cout << "Available extension: " << extension.extensionName << std::endl;
//		requiredExtensions.erase(extension.extensionName);
//	}
//
//	return requiredExtensions.empty();
//}
//
//vk::PhysicalDevice Imp::Render::Device::PickPhysicalDevice(const vk::Instance& instance, const vk::SurfaceKHR& surface)
//{
//	auto devices = instance.enumeratePhysicalDevices();
//	if (devices.empty()) { throw std::runtime_error("Failed to find GPUs with Vulkan support!"); }
//
//	for (const auto& device : devices) { if (IsDeviceSuitable(device, surface)) { return device; } }
//
//	throw std::runtime_error("Failed to find a suitable GPU!");
//}
//
//
//
//vk::UniqueDevice Imp::Render::Device::CreateDevice(const vk::PhysicalDevice& physicalDevice,
//												   const QueueFamilyIndices& indices)
//{
//	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
//	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value(),indices.transferFamily.value() };
//	float queuePriority = 1.0f;
//	queueCreateInfos.reserve(uniqueQueueFamilies.size());
//	for (uint32_t queueFamily : uniqueQueueFamilies) {
//		queueCreateInfos.emplace_back(vk::DeviceQueueCreateInfo({}, queueFamily, 1, &queuePriority));
//	}
//	vk::PhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddressFeatures(true);
//	vk::PhysicalDeviceSynchronization2FeaturesKHR synchronization2Features(true, &bufferDeviceAddressFeatures);
//	vk::PhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeatures(true, &synchronization2Features);
//	
//	vk::PhysicalDeviceFeatures2 deviceFeatures2({},&dynamicRenderingFeatures);
//
//
//	auto layers = std::vector<const char*>();
//	if (vkutil::ENABLE_VALIDATION_LAYERS)
//		layers = vkutil::VALIDATION_LAYERS;
//	vk::DeviceCreateInfo deviceCreateInfo({}, queueCreateInfos, layers, vkutil::DEVICE_EXTENSIONS, {}, &deviceFeatures2);
//	return physicalDevice.createDeviceUnique(deviceCreateInfo);
//}
//
//Imp::Render::Device::Device(const vk::Instance& instance, const vk::SurfaceKHR& surface) :
//	physical(PickPhysicalDevice(instance, surface)),
//	indices(vkutil::FindQueueFamilies(physical, surface)),
//	logical((CreateDevice(physical, indices)))
//{}
//
//Imp::Render::Device::~Device()
//{
//	//std::cout << "Destroying device" << std::endl;
//
//}
//
//Imp::Render::UniqueDevice Imp::Render::CreateUniqueDevice(const Instance& instance, const VKWindow& surface)
//{
//	auto _device = std::unique_ptr< Device>(new Device(instance, surface.getSurface()));
//	
//	VULKAN_HPP_DEFAULT_DISPATCHER.init((*_device).getLogical());
//	return std::move(_device);
//}
