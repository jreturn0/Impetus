//
//#include "core/SwapChain.h"
//
//#include "Debug.h"
//#include "window/Window.h"
//#include "core/Device.h"
//#include "window/VKWindow.h"
//
//
//vk::SurfaceFormatKHR Imp::Render::SwapChain::ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& formats)
//{
//	for (const auto& format : formats) {
//		if (format.format == vk::Format::eR16G16B16A16Sfloat) {
//			//std::cout << "Chosen format: R16G16B16A16_SFLOAT" << std::endl;
//			return format;
//		}
//	}
//	for (const auto& format : formats) {
//		if (format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
//			//std::cout << "Chosen format: B8G8R8A8_SRGB with SRGB nonlinear colorspace" << std::endl;
//			return format;
//		}
//	}
//
//	Debug::Error( "No suitable format found, returning default.");
//	return formats.empty() ? vk::SurfaceFormatKHR{} : formats[0];
//}
//
//
//vk::PresentModeKHR Imp::Render::SwapChain::ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes)
//{
//	for (const auto& availablePresentMode : availablePresentModes) {
//		if (availablePresentMode == vk::PresentModeKHR::eMailbox) { return availablePresentMode; }
//	}
//
//	return vk::PresentModeKHR::eFifo;
//}
//
//vk::Extent2D Imp::Render::SwapChain::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities,
//													  const VKWindow& window)
//{
//	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
//		return capabilities.currentExtent;
//	}
//
//
//	vk::Extent2D actualExtent = window.getExtent();// { static_cast<uint32_t>(), static_cast<uint32_t>(height) };
//
//	actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width,
//									capabilities.maxImageExtent.width);
//	actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height,
//									 capabilities.maxImageExtent.height);
//	return actualExtent;
//}
//
//void Imp::Render::SwapChain::Cleanup()
//{
//	swapChain.reset();
//	swapChain.release();
//	images.clear();
//	imageViews.clear();
//	framebuffers.clear();
//}
//
//void Imp::Render::SwapChain::RecreateSwapChain(const Device& device, const VKWindow& window)
//{
//	device.getLogical().waitIdle();
//	Cleanup();
//	CreateSwapChain(device, window);
//	CreateImageViews(device);
//}
//
//void Imp::Render::SwapChain::CreateSwapChain(const Device& device, const VKWindow& window)
//{
//	SwapChainSupportDetails swapChainSupport = { device, window.getSurface() };
//
//	vk::SurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
//	vk::PresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
//	extent = ChooseSwapExtent(swapChainSupport.capabilities, window);
//	//std::cout << "SwapChain extent: " << extent.width << " " << extent.height << std::endl;
//
//	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
//	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.
//		maxImageCount) {
//		imageCount = swapChainSupport.capabilities.maxImageCount;
//	}
//	
//	 std::set<uint32_t> uniqueQueueFamilies = {
//		device.getIndices().graphicsFamily.value(), device.getIndices().presentFamily.value(),device.getIndices().transferFamily.value()
//	};
//	 std::vector<uint32_t> queueFamilyIndices(uniqueQueueFamilies.begin(), uniqueQueueFamilies.end());
//	 vk::SharingMode sharingMode = (uniqueQueueFamilies.size() > 1) ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive;
//	vk::SwapchainCreateInfoKHR swapchainCreateInfo(
//		{},
//		window.getSurface(),
//		imageCount,
//		surfaceFormat.format,
//		surfaceFormat.colorSpace,
//		extent,
//		1,
//		vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst,
//		sharingMode,
//		queueFamilyIndices,
//		swapChainSupport.capabilities.currentTransform,
//		vk::CompositeAlphaFlagBitsKHR::eOpaque,
//		presentMode,
//		VK_TRUE,
//		nullptr
//	);
//
//	format = surfaceFormat.format;
//
//	swapChain = device.getLogical().createSwapchainKHRUnique(swapchainCreateInfo);
//
//	images = device.getLogical().getSwapchainImagesKHR(swapChain.get());
//}
//
//void Imp::Render::SwapChain::CreateImageViews(const Device& device)
//{
//	imageViews.reserve(images.size());
//	for (const auto& image : images) {
//		vk::ImageViewCreateInfo imageViewCreateInfo(
//			{},
//			image,
//			vk::ImageViewType::e2D,
//			format, // Use the swapChainFormat here
//			{},
//			{ vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 }
//		);
//		imageViews.push_back(device.getLogical().createImageViewUnique(imageViewCreateInfo));
//	}
//}
//
//
//
//Imp::Render::SwapChain::SwapChain(const Device& device, const VKWindow& window)
//{
//	CreateSwapChain(device, window);
//	CreateImageViews(device);
//}
//
//Imp::Render::UniqueSwapChain Imp::Render::CreateUniqueSwapChain(const Device& device,
//																const VKWindow& window)
//{
//	return std::unique_ptr<SwapChain>(new SwapChain( device, window));
//}
