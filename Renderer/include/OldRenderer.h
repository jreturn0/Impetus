//#pragma once
//#define GLFW_INCLUDE_VULKAN
//#include <GLFW/glfw3.h>
//#include <algorithm>
//#include <cstring>
//#include <iostream>
//#include <set>
//#include <sstream>
//#include <optional>
//
//#pragma warning(push)
//#pragma warning(disable: 4996)
//#include "../FileDirectoryHelper.h"
//#include "vulkan/vulkan.hpp"
//#pragma warning(pop)
//
//#define DISABLE_COPY_AND_MOVE(className) \
//    className(const className&) = delete; \
//    className& operator=(const className&) = delete; \
//    className(className&&) = delete; \
//    className& operator=(className&&) = delete;
//
//
//namespace OldImp {
//	class Window
//	{
//		GLFWwindow* glfw;
//		uint32_t width, height;
//		const char* title;
//
//		static GLFWwindow* CreateWindow(uint32_t width, uint32_t height, const char* title)
//		{
//			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
//			glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
//			return glfwCreateWindow(width, height, title, nullptr, nullptr);
//		}
//
//	public:
//		Window(uint32_t width, uint32_t height, const char* title) : glfw(CreateWindow(width, height, title)),
//			width(width), height(height), title(title)
//		{}
//
//		Window(const Window&) = delete;
//		Window& operator=(const Window&) = delete;
//		Window(Window&&) = delete;
//		Window& operator=(Window&&) = delete;
//		operator GLFWwindow* () const { return glfw; }
//		operator GLFWwindow* () { return glfw; }
//
//		//operator const GLFWwindow* ()
//		//{
//		//	return glfw;
//		//}
//	};
//
//	namespace vkutil {
//		const std::vector<const char*> validationLayers = {
//			"VK_LAYER_KHRONOS_validation"
//		};
//		const std::vector<const char*> deviceExtensions = {
//			vk::KHRSwapchainExtensionName
//		};
//
//
//#ifdef NDEBUG
//		const bool enableValidationLayers = false;
//#else
//		const bool enableValidationLayers = true;
//#endif
//		constexpr int MAX_FRAMES_IN_FLIGHT = 2;
//		VKAPI_ATTR inline VkBool32 VKAPI_CALL DebugMessageCallback(
//			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
//			VkDebugUtilsMessageTypeFlagsEXT messageTypes,
//			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
//			void* /*pUserData*/)
//		{
//			std::ostringstream message;
//
//			message << vk::to_string(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(messageSeverity)) << ": "
//				<< vk::to_string(static_cast<vk::DebugUtilsMessageTypeFlagsEXT>(messageTypes)) << ":\n";
//			message << "\tmessageIDName   = <" << pCallbackData->pMessageIdName << ">\n";
//			message << "\tmessageIdNumber = " << pCallbackData->messageIdNumber << "\n";
//			message << "\tmessage         = <" << pCallbackData->pMessage << ">\n";
//
//			if (pCallbackData->queueLabelCount > 0) {
//				message << "\tQueue Labels:\n";
//				for (uint32_t i = 0; i < pCallbackData->queueLabelCount; ++i) {
//					message << "\t\tlabelName = <" << pCallbackData->pQueueLabels[i].pLabelName << ">\n";
//				}
//			}
//
//			if (pCallbackData->cmdBufLabelCount > 0) {
//				message << "\tCommandBuffer Labels:\n";
//				for (uint32_t i = 0; i < pCallbackData->cmdBufLabelCount; ++i) {
//					message << "\t\tlabelName = <" << pCallbackData->pCmdBufLabels[i].pLabelName << ">\n";
//				}
//			}
//
//			if (pCallbackData->objectCount > 0) {
//				message << "\tObjects:\n";
//				for (uint32_t i = 0; i < pCallbackData->objectCount; ++i) {
//					message << "\t\tObject " << i << "\n";
//					message << "\t\t\tobjectType   = " << vk::to_string(
//						static_cast<vk::ObjectType>(pCallbackData->pObjects[i].objectType)) << "\n";
//					message << "\t\t\tobjectHandle = " << pCallbackData->pObjects[i].objectHandle << "\n";
//					if (pCallbackData->pObjects[i].pObjectName) {
//						message << "\t\t\tobjectName   = <" << pCallbackData->pObjects[i].pObjectName << ">\n";
//					}
//				}
//			}
//
//			std::cout << message.str() << std::endl;
//
//			return VK_FALSE;
//		}
//
//		struct QueueFamilyIndices
//		{
//			std::optional<uint32_t> graphicsFamily;
//			std::optional<uint32_t> presentFamily;
//
//			bool isComplete() const { return graphicsFamily.has_value() && presentFamily.has_value(); }
//		};
//
//		struct SwapChainSupportDetails
//		{
//			vk::SurfaceCapabilitiesKHR capabilities;
//			std::vector<vk::SurfaceFormatKHR> formats;
//			std::vector<vk::PresentModeKHR> presentModes;
//		};
//
//		inline std::vector<const char*> GetRequiredExtensions()
//		{
//			uint32_t glfwExtensionCount = 0;
//			const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
//			std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
//			if (enableValidationLayers) { extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME); }
//			return extensions;
//		}
//
//		inline bool CheckValidationLayerSupport()
//		{
//			std::vector<vk::LayerProperties> availableLayers = vk::enumerateInstanceLayerProperties();
//			for (const char* layerName : validationLayers) {
//				bool layerFound = false;
//				for (const auto& layerProperties : availableLayers) {
//					if (strcmp(layerName, layerProperties.layerName) == 0) {
//						layerFound = true;
//						break;
//					}
//				}
//				if (!layerFound) {
//					std::cerr << "Validation layer not available: " << layerName << std::endl;
//					return false;
//				}
//			}
//			return true;
//		}
//
//		inline bool CheckExtensionsAvailability(const std::vector<const char*>& requiredExtensions)
//		{
//			auto availableExtensions = vk::enumerateInstanceExtensionProperties();
//			std::set<std::string> availableExtensionsSet;
//			for (const auto& extension : availableExtensions) {
//				availableExtensionsSet.insert(extension.extensionName);
//			}
//
//			for (const auto& required : requiredExtensions) {
//				std::cout << "Required extension: " << required << std::endl;
//				if (availableExtensionsSet.find(required) == availableExtensionsSet.end()) {
//					std::cerr << "Required extension not available: " << required << std::endl;
//					return false;
//				}
//			}
//			return true;
//		}
//
//
//		inline vk::UniqueInstance CreateInstance(const char* title)
//		{
//			auto version = VK_MAKE_API_VERSION(0, 1, 0, 0);
//			vk::ApplicationInfo appInfo(
//				title,
//				version,
//				"Impetus",
//				version,
//				VK_API_VERSION_1_3
//			);
//
//			std::vector<const char*> extensions = GetRequiredExtensions();
//			//extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
//			if (!CheckExtensionsAvailability(extensions)) {
//				throw std::runtime_error("Required extensions are not available");
//			}
//			if (enableValidationLayers && !CheckValidationLayerSupport()) {
//				throw std::runtime_error("Validation layers are not available");
//			}
//
//			vk::InstanceCreateInfo instanceCreateInfo(
//				{},
//				&appInfo,
//				enableValidationLayers ? static_cast<uint32_t>(validationLayers.size()) : 0,
//				enableValidationLayers ? validationLayers.data() : nullptr,
//				static_cast<uint32_t>(extensions.size()),
//				extensions.data()
//			);
//
//			return vk::createInstanceUnique(instanceCreateInfo);
//		}
//
//		inline vk::UniqueHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic> CreateDebugUtilsMessenger(
//			const vk::Instance& instance, const vk::DispatchLoaderDynamic& dldi)
//		{
//			vk::DebugUtilsMessengerCreateInfoEXT createInfo(
//				{},
//				vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
//				vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo,
//				vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
//				vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
//				DebugMessageCallback
//			);
//			return instance.createDebugUtilsMessengerEXTUnique(createInfo, nullptr, dldi);
//		}
//
//		inline QueueFamilyIndices FindQueueFamilies(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface)
//		{
//			QueueFamilyIndices indices;
//
//			auto queueFamilies = device.getQueueFamilyProperties();
//
//			int i = 0;
//			for (const auto& queueFamily : queueFamilies) {
//				if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) { indices.graphicsFamily = i; }
//				VkBool32 presentSupport = device.getSurfaceSupportKHR(i, surface);
//				if (presentSupport) { indices.presentFamily = i; }
//
//				if (indices.isComplete()) { break; }
//
//				i++;
//			}
//
//			return indices;
//		}
//
//		inline bool CheckDeviceExtensionSupport(const vk::PhysicalDevice& device)
//		{
//			auto extProps = device.enumerateDeviceExtensionProperties();
//			std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
//
//			for (const auto& extension : extProps) {
//				std::cout << "Available extension: " << extension.extensionName << std::endl;
//				requiredExtensions.erase(extension.extensionName);
//			}
//
//			return requiredExtensions.empty();
//		}
//
//
//		inline SwapChainSupportDetails
//			QuerySwapChainSupport(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface)
//		{
//			return {
//				device.getSurfaceCapabilitiesKHR(surface), device.getSurfaceFormatsKHR(surface),
//				device.getSurfacePresentModesKHR(surface)
//			};
//		}
//
//
//		inline bool IsDeviceSuitable(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface)
//		{
//			QueueFamilyIndices indices = FindQueueFamilies(device, surface);
//
//			bool extensionsSupported = CheckDeviceExtensionSupport(device);
//			SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device, surface);
//			bool swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
//			vk::PhysicalDeviceFeatures supportedFeatures = device.getFeatures();
//
//			return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.
//				samplerAnisotropy;
//		}
//
//		inline vk::PhysicalDevice PickPhysicalDevice(vk::Instance instance, const vk::SurfaceKHR& surface)
//		{
//			auto devices = instance.enumeratePhysicalDevices();
//			if (devices.empty()) { throw std::runtime_error("Failed to find GPUs with Vulkan support!"); }
//
//			for (const auto& device : devices) { if (IsDeviceSuitable(device, surface)) { return device; } }
//
//			throw std::runtime_error("Failed to find a suitable GPU!");
//		}
//
//		inline vk::UniqueDevice CreateDevice(const vk::PhysicalDevice& physicalDevice,
//											 const QueueFamilyIndices& indices)
//		{
//			std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
//			std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };
//			float queuePriority = 1.0f;
//			for (uint32_t queueFamily : uniqueQueueFamilies) {
//				queueCreateInfos.emplace_back(vk::DeviceQueueCreateInfo({}, queueFamily, 1, &queuePriority));
//			}
//
//			vk::PhysicalDeviceFeatures deviceFeatures{};
//			auto layers = std::vector<const char*>();
//			if (enableValidationLayers)
//				layers = validationLayers;
//			vk::DeviceCreateInfo deviceCreateInfo({}, queueCreateInfos, layers, deviceExtensions, &deviceFeatures);
//			return physicalDevice.createDeviceUnique(deviceCreateInfo);
//		}
//
//		class Device
//		{
//		private:
//			vk::PhysicalDevice physical;
//			QueueFamilyIndices indices;
//			vk::UniqueDevice logical;
//
//		public:
//			DISABLE_COPY_AND_MOVE(Device);
//			// Getter for non-const physical device
//			vk::PhysicalDevice& getPhysical() { return physical; }
//
//			// Getter for const physical device
//			const vk::PhysicalDevice& getPhysical() const { return physical; }
//
//			// Getter for non-const logical device
//			vk::Device& getLogical() { return logical.get(); }
//
//			// Getter for const logical device
//			const vk::Device& getLogical() const { return logical.get(); }
//
//			// Getter for non-const indices
//			QueueFamilyIndices& getIndices() { return indices; }
//
//			// Getter for const indices
//			const QueueFamilyIndices& getIndices() const { return indices; }
//
//			operator vk::PhysicalDevice& () { return physical; }
//			operator const vk::PhysicalDevice& () const { return physical; }
//			operator vk::Device& () { return logical.get(); }
//			operator const vk::Device& () const { return logical.get(); }
//
//			Device(const vk::Instance& instance, const vk::SurfaceKHR& surface)
//			{
//				physical = PickPhysicalDevice(instance, surface);
//				indices = FindQueueFamilies(physical, surface);
//				logical = CreateDevice(physical, indices);
//			}
//		};
//
//		using UniqueDevice = std::unique_ptr<Device>;
//
//		inline UniqueDevice CreateUniqueDevice(const vk::Instance& instance, const vk::SurfaceKHR& surface)
//		{
//			return std::make_unique<Device>(instance, surface);
//		}
//
//		inline vk::UniqueSurfaceKHR CreateSurface(vk::Instance instance, GLFWwindow* window)
//		{
//			VkSurfaceKHR surface;
//			if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
//				throw std::runtime_error("Failed to create window surface!");
//			}
//
//			return vk::UniqueSurfaceKHR(surface, instance);
//		}
//
//		inline vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> formats)
//		{
//			for (auto&& format : formats) {
//				if (format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace ==
//					vk::ColorSpaceKHR::eSrgbNonlinear) {
//					return format;
//				}
//			}
//			return {};
//		}
//
//		inline vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes)
//		{
//			for (const auto& availablePresentMode : availablePresentModes) {
//				if (availablePresentMode == vk::PresentModeKHR::eMailbox) { return availablePresentMode; }
//			}
//
//			return vk::PresentModeKHR::eFifo;
//		}
//
//		inline vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, const Window& window)
//		{
//			if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
//				return capabilities.currentExtent;
//			}
//			int width, height;
//			glfwGetFramebufferSize(window, &width, &height);
//
//			vk::Extent2D actualExtent{ static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
//
//			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width,
//											capabilities.maxImageExtent.width);
//			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height,
//											 capabilities.maxImageExtent.height);
//			return actualExtent;
//		}
//
//
//		class SwapChain
//		{
//		private:
//			SwapChainSupportDetails details;
//			vk::UniqueSwapchainKHR swapChain;
//			std::vector<vk::Image> images;
//			std::vector<vk::UniqueImageView> imageViews;
//			vk::Format format;
//			vk::Extent2D extent;
//			std::vector<vk::UniqueFramebuffer> framebuffers;
//
//		public:
//			[[nodiscard]] const SwapChainSupportDetails& getDetails() const { return details; }
//
//			[[nodiscard]] const vk::SwapchainKHR& getSwapChain() const { return *swapChain; }
//
//			[[nodiscard]] const std::vector<vk::Image>& getImages() const { return images; }
//
//			[[nodiscard]] const std::vector<vk::UniqueImageView>& getImageViews() const { return imageViews; }
//
//			[[nodiscard]] const std::vector<vk::UniqueFramebuffer>& getFramebuffers() const { return framebuffers; }
//
//			[[nodiscard]] vk::Format getFormat() const { return format; }
//
//			[[nodiscard]] const vk::Extent2D& getExtent() const { return extent; }
//
//			operator const vk::SwapchainKHR() const { return swapChain.get(); }
//			operator vk::SwapchainKHR() { return swapChain.get(); }
//
//
//			void Cleanup()
//			{
//				swapChain.reset();
//				swapChain.release();
//				images.clear();
//				imageViews.clear();
//				framebuffers.clear();
//			}
//			void RecreateSwapChain(const Device& device, const vk::SurfaceKHR& surface, const Window& window,
//								   const vk::RenderPass& renderPass)
//			{
//				int width = 0, height = 0;
//				glfwGetFramebufferSize(window, &width, &height);
//				while (width == 0 || height == 0) {
//					glfwGetFramebufferSize(window, &width, &height);
//					glfwWaitEvents();
//				}
//				device.getLogical().waitIdle();
//				Cleanup();
//				CreateSwapChain(device, surface, window);
//				CreateImageViews(device);
//				GenerateFramebuffers(device, renderPass);
//			}
//
//
//			void CreateSwapChain(const Device& device, const vk::SurfaceKHR& surface, const Window& window)
//			{
//				SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device, surface);
//
//				vk::SurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
//				vk::PresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
//				extent = ChooseSwapExtent(swapChainSupport.capabilities, window);
//
//				uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
//				if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.
//					maxImageCount) {
//					imageCount = swapChainSupport.capabilities.maxImageCount;
//				}
//
//				uint32_t queueFamilyIndices[] = {
//					device.getIndices().graphicsFamily.value(), device.getIndices().presentFamily.value()
//				};
//
//				vk::SwapchainCreateInfoKHR swapchainCreateInfo(
//					{},
//					surface,
//					imageCount,
//					surfaceFormat.format,
//					surfaceFormat.colorSpace,
//					extent,
//					1,
//					vk::ImageUsageFlagBits::eColorAttachment,
//					(device.getIndices().graphicsFamily != device.getIndices().presentFamily)
//					? vk::SharingMode::eConcurrent
//					: vk::SharingMode::eExclusive,
//					(device.getIndices().graphicsFamily != device.getIndices().presentFamily) ? 2 : 0,
//					(device.getIndices().graphicsFamily != device.getIndices().presentFamily)
//					? queueFamilyIndices
//					: nullptr,
//					swapChainSupport.capabilities.currentTransform,
//					vk::CompositeAlphaFlagBitsKHR::eOpaque,
//					presentMode,
//					VK_TRUE,
//					nullptr
//				);
//				
//				format = surfaceFormat.format;
//
//				swapChain = device.getLogical().createSwapchainKHRUnique(swapchainCreateInfo);
//
//				images = device.getLogical().getSwapchainImagesKHR(swapChain.get());
//			}
//
//			void CreateImageViews(const vk::Device& device)
//			{
//				imageViews.reserve(images.size());
//				for (const auto& image : images) {
//					vk::ImageViewCreateInfo imageViewCreateInfo(
//						{},
//						image,
//						vk::ImageViewType::e2D,
//						format, // Use the swapChainFormat here
//						{},
//						{ vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 }
//					);
//					imageViews.push_back(device.createImageViewUnique(imageViewCreateInfo));
//				}
//			}
//
//			void GenerateFramebuffers(const vk::Device& device, const vk::RenderPass& renderPass)
//			{
//				for (const auto& imageView : imageViews) {
//					vk::ImageView attachments[] = {
//						*imageView
//					};
//
//					vk::FramebufferCreateInfo framebufferInfo({}, renderPass, attachments, extent.width, extent.height,
//															  1);
//					framebuffers.push_back(device.createFramebufferUnique(framebufferInfo));
//				}
//			}
//
//			SwapChain(const Device& device, const vk::SurfaceKHR& surface, const Window& window)
//			{
//				CreateSwapChain(device, surface, window);
//				CreateImageViews(device);
//			}
//		};
//
//		using UniqueSwapChain = std::unique_ptr<SwapChain>;
//
//		inline UniqueSwapChain CreateUniqueSwapChain(const Device& device, const vk::SurfaceKHR& surface,
//													 const Window& window)
//		{
//			return std::make_unique<SwapChain>(device, surface, window);
//		}
//
//		enum class ShaderStage
//		{
//			Vertex = 0,
//			Fragment = 1,
//			Compute = 2,
//			Geometry = 3
//		};
//
//		std::vector<uint32_t> CompileShader(const std::string& source, ShaderStage kind, const std::string& shaderName);
//
//
//		inline vk::UniqueShaderModule CreateShaderModule(const std::vector<uint32_t>& code, const vk::Device& device)
//		{
//			vk::ShaderModuleCreateInfo createInfo{
//				{},
//				code.size() * sizeof(uint32_t),
//				code.data()
//			};
//
//			return device.createShaderModuleUnique(createInfo);
//		}
//
//		inline vk::UniqueShaderModule CompileAndCreateShaderModule(const vk::Device& device, const std::string& source,
//																   ShaderStage kind)
//		{
//			auto it = source.find_last_of('/');
//			it = it == std::string::npos ? source.find_last_of('\\') : it;
//			it = it == std::string::npos ? 0 : it;
//			const auto name = source.substr(it);
//			const auto code = CompileShader(source, kind, name);
//			return CreateShaderModule(code, device);
//		}
//
//		struct Pipeline
//		{
//			vk::UniquePipelineLayout layout;
//			vk::UniquePipeline pipeline;
//		};
//
//		using UniquePipeline = std::unique_ptr<Pipeline>;
//
//		inline auto CreatePipeline(const Device& device, const SwapChain& swapChain, const vk::RenderPass& renderPass,
//								   const std::string& source)
//		{
//			FileDirectoryHelper fileDir;
//			fileDir.gotoChildDirectory("Shaders");
//			//std::cout << fileDir.getCurrentPath().string()<<std::endl;
//
//
//			auto vertFile = std::string(source).append(".vert");
//			if (!fileDir.getFiles().contains(vertFile)) { throw std::runtime_error(vertFile.append(" not found")); }
//			auto vertFileSource = fileDir.getFilePath(vertFile);
//			std::cout << "VertFile: " << vertFileSource << std::endl;
//			auto vert = CompileAndCreateShaderModule(device, vertFileSource, ShaderStage::Vertex);
//
//			auto fragFile = std::string(source).append(".frag");
//			if (!fileDir.getFiles().contains(fragFile)) { throw std::runtime_error(fragFile.append(" not found")); }
//			auto fragFileSource = fileDir.getFilePath(fragFile);
//			auto frag = CompileAndCreateShaderModule(device, fragFileSource, ShaderStage::Fragment);
//
//			//vk::PipelineShaderStageCreateInfo vertexStage;
//			//vk::PipelineShaderStageCreateInfo fragmentStage;
//			vk::PipelineShaderStageCreateInfo shaderStages[] = {
//				{{}, vk::ShaderStageFlagBits::eVertex, *vert, "main"},
//				{{}, vk::ShaderStageFlagBits::eFragment, *frag, "main"}
//			};
//
//			std::vector<vk::DynamicState> dynamicStates = {
//				vk::DynamicState::eViewport,
//				vk::DynamicState::eScissor
//			};
//
//			vk::PipelineDynamicStateCreateInfo dynamicStateInfo({}, dynamicStates);
//
//			vk::PipelineVertexInputStateCreateInfo vertexInputInfo({});
//
//			vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo({}, vk::PrimitiveTopology::eTriangleList, false);
//
//			vk::Viewport viewport(0, 0, swapChain.getExtent().width, swapChain.getExtent().height, 0.f, 1.f);
//
//			vk::Rect2D scissor({ 0, 0 }, swapChain.getExtent());
//
//			vk::PipelineViewportStateCreateInfo viewportInfo({}, viewport, scissor);
//
//			vk::PipelineRasterizationStateCreateInfo rasterizationInfo({}, false, false, vk::PolygonMode::eFill,
//																	   vk::CullModeFlagBits::eBack,
//																	   vk::FrontFace::eClockwise, false, 0.f, 0.f, 0.f,
//																	   1.f);
//
//			vk::PipelineMultisampleStateCreateInfo multisampleInfo({}, vk::SampleCountFlagBits::e1, false, 1.f, {},
//																   false, false);
//
//			vk::PipelineDepthStencilStateCreateInfo depthStencilInfo;
//
//			vk::PipelineColorBlendAttachmentState colorBlendAttachment(false, vk::BlendFactor::eOne,
//																	   vk::BlendFactor::eZero, vk::BlendOp::eAdd,
//																	   vk::BlendFactor::eOne, vk::BlendFactor::eZero,
//																	   vk::BlendOp::eAdd,
//																	   vk::ColorComponentFlagBits::eR |
//																	   vk::ColorComponentFlagBits::eG |
//																	   vk::ColorComponentFlagBits::eB |
//																	   vk::ColorComponentFlagBits::eA);
//
//			vk::PipelineColorBlendStateCreateInfo colorInfo({}, false, vk::LogicOp::eCopy, colorBlendAttachment,
//															{ 0.f, 0.f, 0.f, 0.f });
//
//			vk::PipelineLayoutCreateInfo layoutInfo({}, {}, {});
//
//			vk::UniquePipelineLayout layout = device.getLogical().createPipelineLayoutUnique(layoutInfo);
//
//			vk::GraphicsPipelineCreateInfo pipelineInfo({}, shaderStages, &vertexInputInfo, &inputAssemblyInfo, {},
//														&viewportInfo, &rasterizationInfo, &multisampleInfo,
//														&depthStencilInfo, &colorInfo, &dynamicStateInfo, layout.get(),
//														renderPass, 0, {}, -1);
//			auto result = device.getLogical().createGraphicsPipelineUnique({}, pipelineInfo);
//			if (result.result != vk::Result::eSuccess) {
//				throw std::runtime_error("Failed to create graphics pipeline");
//			}
//
//			// Extract the unique pipeline
//			vk::UniquePipeline pipeline = std::move(result.value);
//
//			// Return the pipeline struct
//			return std::make_unique<Pipeline>(std::move(layout), std::move(pipeline));
//		}
//
//		class RenderPass
//		{
//		private:
//			SwapChain& swapChain;
//			vk::UniqueRenderPass renderPass;
//
//
//			vk::UniqueRenderPass CreateRenderPass(const Device& device) const
//			{
//				vk::AttachmentDescription colorAttachment({}, swapChain.getFormat(),
//														  vk::SampleCountFlagBits::e1,
//														  vk::AttachmentLoadOp::eClear,
//														  vk::AttachmentStoreOp::eStore,
//														  vk::AttachmentLoadOp::eDontCare,
//														  vk::AttachmentStoreOp::eDontCare,
//														  vk::ImageLayout::eUndefined,
//														  vk::ImageLayout::ePresentSrcKHR
//				);
//
//				vk::AttachmentReference colorAttachRef(
//					0,
//					vk::ImageLayout::eColorAttachmentOptimal
//				);
//
//				vk::SubpassDescription subpass(
//					{},
//					vk::PipelineBindPoint::eGraphics,
//					{},
//					colorAttachRef
//				);
//
//				vk::SubpassDependency dependency(
//					0,
//					0,
//					vk::PipelineStageFlagBits::eColorAttachmentOutput,
//					vk::PipelineStageFlagBits::eColorAttachmentOutput,
//					vk::AccessFlagBits::eNone,
//					vk::AccessFlagBits::eColorAttachmentWrite,
//					vk::DependencyFlagBits::eByRegion // Add this flag
//				);
//
//				vk::RenderPassCreateInfo renderPassInfo(
//					{},
//					colorAttachment,
//					subpass,
//					dependency
//				);
//
//				return device.getLogical().createRenderPassUnique(renderPassInfo);
//			}
//
//		public:
//			const vk::RenderPass& get() const { return *renderPass; }
//
//			RenderPass(const Device& device, SwapChain& swapChain) : swapChain(swapChain),
//				renderPass(CreateRenderPass(device))
//			{
//				swapChain.GenerateFramebuffers(device, renderPass.get());
//			}
//
//			void beginRenderPass(const vk::CommandBuffer& buffer, const uint32_t index)
//			{
//				vk::ClearValue clearValue;
//				clearValue.color = vk::ClearColorValue(std::array<float, 4>{0.f, 0.f, 0.f, 1.f});
//				vk::RenderPassBeginInfo beginInfo(*renderPass, *swapChain.getFramebuffers()[index],
//												  { {}, swapChain.getExtent() }, clearValue);
//				buffer.beginRenderPass(beginInfo, vk::SubpassContents::eInline);
//			}
//		};
//
//		using UniqueRenderPass = std::unique_ptr<RenderPass>;
//
//		inline UniqueRenderPass CreateUniqueRenderPass(const Device& device, SwapChain& swapChain)
//		{
//			return std::make_unique<RenderPass>(device, swapChain);
//		}
//
//		inline vk::UniqueRenderPass CreateRenderPass(const Device& device, SwapChain& swapChain)
//		{
//			vk::AttachmentDescription colorAttachment({}, swapChain.getFormat(), vk::SampleCountFlagBits::e1,
//													  vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
//													  vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare,
//													  vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR);
//			vk::AttachmentReference colorAttachRef(0, vk::ImageLayout::eColorAttachmentOptimal);
//			vk::SubpassDescription subpass({}, vk::PipelineBindPoint::eGraphics, {}, colorAttachRef);
//			vk::RenderPassCreateInfo renderPassInfo({}, colorAttachment, subpass);
//
//			auto renderPass = device.getLogical().createRenderPassUnique(renderPassInfo);
//
//			swapChain.GenerateFramebuffers(device, renderPass.get());
//
//			return renderPass;
//		}
//
//		inline vk::UniqueCommandPool CreateCommandPool(const Device& device)
//		{
//			vk::CommandPoolCreateInfo commandPoolInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
//													  device.getIndices().graphicsFamily.value());
//			return device.getLogical().createCommandPoolUnique(commandPoolInfo);
//		}
//
//		inline vk::UniqueCommandBuffer CreateCommandBuffer(const Device& device, const vk::CommandPool& commandPool)
//		{
//			vk::CommandBufferAllocateInfo allocInfo(commandPool, vk::CommandBufferLevel::ePrimary, 1);
//			return std::move(device.getLogical().allocateCommandBuffersUnique(allocInfo)[0]);
//		}
//
//		using UniqueCommandBufferArray = std::array<vk::UniqueCommandBuffer, MAX_FRAMES_IN_FLIGHT>;
//
//
//
//		class CommandBuffer
//		{
//			UniqueCommandBufferArray commandBuffers;
//
//			UniqueCommandBufferArray fillArray(const vk::Device& device, const vk::CommandPool& commandPool)
//			{
//				UniqueCommandBufferArray bufferArray;
//				auto buffers= device.allocateCommandBuffersUnique({ commandPool, vk::CommandBufferLevel::ePrimary, MAX_FRAMES_IN_FLIGHT });
//				for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
//					bufferArray[i] = std::move(buffers[i]);
//				}
//				return (bufferArray);
//			}
//
//		public:
//			DISABLE_COPY_AND_MOVE(CommandBuffer);
//
//			CommandBuffer(const vk::Device& device, const vk::CommandPool& commandPool) :
//				commandBuffers(fillArray(device, commandPool))
//			{}
//
//			const vk::CommandBuffer& operator [](const int frameIndex) const { return *commandBuffers[frameIndex]; }
//
//			vk::CommandBuffer& operator[](uint32_t index) { return *commandBuffers[index]; }
//			//vk::CommandBuffer& getBuffer() { return commandBuffer.get(); }
//			const UniqueCommandBufferArray& getBuffers() const { return commandBuffers; }
//			const vk::CommandBuffer& getBuffer(uint32_t frameIndex) const { return *commandBuffers[frameIndex]; }
//
//
//			void beginRecording(uint32_t frameIndex)
//			{
//				vk::CommandBufferBeginInfo beginInfo;
//				commandBuffers[frameIndex]->begin(beginInfo);
//			}
//		};
//
//		using UniqueCommandBuffer = std::unique_ptr<CommandBuffer>;
//
//
//		inline UniqueCommandBuffer CreateUniqueCommandBuffer(const vk::Device& device,
//															 const vk::CommandPool& commandPool)
//		{
//			return std::make_unique<CommandBuffer>(device, commandPool);
//		}
//
//		//inline UniqueCommandBufferArray CreateUniqueCommandBufferArray(const vk::Device& device, const vk::CommandPool& commandPool)
//		//{
//		//	UniqueCommandBufferArray bufferArray;
//		//	bufferArray.fill(CreateUniqueCommandBuffer(device,commandPool));
//		//	return std::move(bufferArray);
//		//}
//
//
//		using UniqueSemaphoreArray = std::array<vk::UniqueSemaphore, MAX_FRAMES_IN_FLIGHT>;
//		using UniqueFenceArray = std::array<vk::UniqueFence, MAX_FRAMES_IN_FLIGHT>;
//
//		class SyncObjects
//		{
//		private:
//			UniqueSemaphoreArray imageAvailable;
//			UniqueSemaphoreArray renderFinished;
//			UniqueFenceArray inFlightFences;
//			UniqueFenceArray inFlightImages;
//
//		public:
//			const UniqueSemaphoreArray& getImageAvailable() const { return imageAvailable; }
//
//			const UniqueSemaphoreArray& getRenderFinished() const { return renderFinished; }
//
//			const UniqueFenceArray& getInFlightFences() const { return inFlightFences; }
//
//			const UniqueFenceArray& getInFlightImages() const { return inFlightImages; }
//
//
//			const vk::Semaphore& getImageAvailable(uint32_t frameIndex) const { return *imageAvailable[frameIndex]; }
//			const vk::Semaphore& getRenderFinished(uint32_t frameIndex) const { return *renderFinished[frameIndex]; }
//			const vk::Fence& getInFlightFences(uint32_t frameIndex) const { return *inFlightFences[frameIndex]; }
//			const vk::Fence& getInFlightImages(uint32_t frameIndex) const { return *inFlightImages[frameIndex]; }
//
//
//			SyncObjects(const vk::Device& device)
//			{
//				vk::SemaphoreCreateInfo semaphoreInfo;
//				vk::FenceCreateInfo fenceInfo({ vk::FenceCreateFlagBits::eSignaled });
//
//				for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
//					imageAvailable[i] = device.createSemaphoreUnique(semaphoreInfo);
//					renderFinished[i] = device.createSemaphoreUnique(semaphoreInfo);
//					inFlightFences[i] = device.createFenceUnique(fenceInfo);
//					inFlightImages[i] = device.createFenceUnique(fenceInfo);
//				}
//			}
//		};
//
//		using UniqueSyncObjects = std::unique_ptr<SyncObjects>;
//
//		inline UniqueSyncObjects CreateUniqueSyncObjects(const vk::Device& device)
//		{
//			return std::make_unique<SyncObjects>(device);
//		}
//	}
//
//	class Renderer
//	{
//	private:
//		bool initialConstruct()
//		{
//			glfwInit();
//			return true;
//		}
//
//		bool initialConstructFlag;
//		vk::UniqueInstance instance;
//		Window window;
//		vk::UniqueSurfaceKHR surface;
//		vk::DispatchLoaderDynamic dldi;
//		vk::UniqueHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic> debugMessenger;
//		vkutil::UniqueDevice device;
//		vk::Queue graphicsQueue;
//		vk::Queue presentQueue;
//		vkutil::UniqueSwapChain swapChain;
//		vkutil::UniqueRenderPass renderPass;
//		vkutil::UniquePipeline pipeline;
//		vk::UniqueCommandPool commandPool;
//		vkutil::UniqueCommandBuffer commandBuffer;
//		vkutil::UniqueSyncObjects syncObjects;
//		uint32_t currentFrame = 0;
//		bool isFramebufferResized;
//
//	public:
//		void framebufferResized() { isFramebufferResized = true; }
//
//		Renderer(uint32_t width, uint32_t height, const char* title) :
//			initialConstructFlag(initialConstruct()),
//			instance(vkutil::CreateInstance(title)),
//			window(width, height, title),
//			surface(vkutil::CreateSurface(instance.get(), window)),
//			dldi(instance.get(), vkGetInstanceProcAddr),
//			debugMessenger(vkutil::CreateDebugUtilsMessenger(instance.get(), dldi)),
//			device(vkutil::CreateUniqueDevice(instance.get(), surface.get())),
//			graphicsQueue(device->getLogical(). getQueue(device->getIndices().graphicsFamily.value(), 0)),
//			presentQueue(device->getLogical().getQueue(device->getIndices().presentFamily.value(), 0)),
//			swapChain(vkutil::CreateUniqueSwapChain(*device, surface.get(), window)),
//			renderPass(vkutil::CreateUniqueRenderPass(*device.get(), *swapChain.get())),
//			pipeline(vkutil::CreatePipeline(*device.get(), *swapChain.get(), renderPass->get(), "triangle")),
//			commandPool(vkutil::CreateCommandPool(*device.get())),
//			commandBuffer(vkutil::CreateUniqueCommandBuffer(*device.get(), commandPool.get())),
//			syncObjects(vkutil::CreateUniqueSyncObjects(*device.get()))
//		{
//			//	vkutil::CreateInvalidInstance();
//		}
//
//
//
//		void recordHelloTriangle(uint32_t imageIndex)
//		{
//			if (imageIndex >= commandBuffer->getBuffers().size()) {
//				//throw std::runtime_error("Image index out of range!");
//			}
//			auto& commandBuf = (*commandBuffer)[currentFrame];
//
//			vk::CommandBufferBeginInfo beginInfo(
//				vk::CommandBufferUsageFlagBits::eSimultaneousUse
//			);
//			if (commandBuf.begin(&beginInfo) != vk::Result::eSuccess) {
//				throw std::runtime_error("Failed to begin recording command buffer!");
//			}
//
//			std::array<vk::ClearValue, 1> clearValues = {
//				vk::ClearColorValue(std::array<float, 4>({0.0f, 0.0f, 0.0f, 1.0f}))
//			};
//
//			vk::RenderPassBeginInfo renderPassInfo(
//				renderPass->get(),
//				*swapChain->getFramebuffers()[imageIndex],
//				vk::Rect2D(vk::Offset2D(0, 0), swapChain->getExtent()),
//				clearValues.size(),
//				clearValues.data()
//			);
//
//			commandBuf.beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);
//
//			commandBuf.bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline.get()->pipeline);
//
//			vk::Viewport viewport(0.0f, 0.0f, static_cast<float>(swapChain->getExtent().width), static_cast<float>(swapChain->getExtent().height), 0.0f, 1.0f);
//			commandBuf.setViewport(0, 1, &viewport);
//
//			vk::Rect2D scissor(vk::Offset2D(0, 0), swapChain->getExtent());
//			commandBuf.setScissor(0, 1, &scissor);
//
//			commandBuf.draw(3, 1, 0, 0);
//
//			commandBuf.endRenderPass();
//			commandBuf.end();
//		}
//
//		void drawHelloTriangle()
//		{
//			auto& logical = device->getLogical();
//			auto& flightFence = syncObjects->getInFlightFences(currentFrame);
//			auto& renderFinished = syncObjects->getRenderFinished(currentFrame);
//			auto& imageAvailable = syncObjects->getImageAvailable(currentFrame);
//
//			logical.waitForFences(flightFence, VK_TRUE, UINT64_MAX);
//
//			uint32_t imageIndex = 0;
//			vk::Result result = logical.acquireNextImageKHR(*swapChain.get(), UINT64_MAX, imageAvailable, {}, &imageIndex);
//
//			if (result == vk::Result::eErrorOutOfDateKHR) {
//				swapChain->RecreateSwapChain(*device, *surface, window, renderPass->get());
//				isFramebufferResized = false;
//				return;
//			} else if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) {
//				throw std::runtime_error("Failed to acquire swap chain image!");
//			}
//
//			logical.resetFences(flightFence);
//
//			auto& commandBuf = (*commandBuffer)[currentFrame];
//			commandBuf.reset({});
//
//			recordHelloTriangle(imageIndex);
//
//			vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
//			vk::SubmitInfo submitInfo(
//				1, &imageAvailable, // waitSemaphoreCount, pWaitSemaphores
//				&waitStage,         // pWaitDstStageMask
//				1, &commandBuf,     // commandBufferCount, pCommandBuffers
//				1, &renderFinished  // signalSemaphoreCount, pSignalSemaphores
//			);
//
//			if (graphicsQueue.submit(1, &submitInfo, flightFence) != vk::Result::eSuccess) {
//				throw std::runtime_error("Failed to submit draw command buffer!");
//			}
//
//			vk::PresentInfoKHR presentInfo(
//				1, &renderFinished,        // waitSemaphoreCount, pWaitSemaphores
//				1, &swapChain->getSwapChain(), // swapchainCount, pSwapchains
//				&imageIndex                // pImageIndices
//			);
//
//			result = presentQueue.presentKHR(&presentInfo);
//			if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR) {
//				swapChain->RecreateSwapChain(*device, *surface, window, renderPass->get());
//				isFramebufferResized = false;
//			} else if (result != vk::Result::eSuccess) {
//				throw std::runtime_error("Failed to present swap chain image!");
//			}
//
//			currentFrame = (currentFrame++) % (vkutil::MAX_FRAMES_IN_FLIGHT);
//		}
//
//
//
//		void endRecording()
//		{}
//
//
//		~Renderer()
//		{
//			device->getLogical().waitIdle();
//			glfwDestroyWindow(window);
//			glfwTerminate();
//		}
//
//		Window& getWindow() { return window; }
//	};
//}
