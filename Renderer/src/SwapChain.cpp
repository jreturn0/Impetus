#include "Core/SwapChain.h"
#include <window/Window.h>
#include "utils/VKUtils.h"
namespace {



    vk::SwapchainCreateInfoKHR getSwapChainCreateInfo(SwapChainSupportDetails& details, const vk::SurfaceFormatKHR& surfaceFormat, const vk::Extent2D& extent, vk::PresentModeKHR presentMode, const imp::gfx::VulkanContext& context)
    {
        auto minImageCount = std::max(imp::gfx::vkutil::MIN_IMAGE_COUNT, details.capabilities.minImageCount);
        minImageCount = (details.capabilities.maxImageCount > 0 && minImageCount > details.capabilities.maxImageCount) ? details.capabilities.maxImageCount : minImageCount;
        uint32_t imageCount = details.capabilities.minImageCount + 1;
        if (details.capabilities.maxImageCount > 0 && imageCount > details.capabilities.
            maxImageCount) {
            imageCount = details.capabilities.maxImageCount;
        }


        static std::array<uint32_t, 2> indices{}; // hacky way to avoid returning a reference to a local variable
        const auto& queueFamilyIndices = context.getQueueFamilyIndices();
        if (!queueFamilyIndices.isComplete2()) {
            Debug::FatalError("Queue family indices are not complete!");
        }
        indices = { queueFamilyIndices.graphicsFamily.value(),queueFamilyIndices.presentFamily.value() };
        const bool sameQueueFamily = queueFamilyIndices.graphicsFamily == queueFamilyIndices.presentFamily;
        if (!sameQueueFamily) 
            Debug::Info("Using concurrent sharing mode for swapchain");
        const auto swapchainCreateInfo = vk::SwapchainCreateInfoKHR{}
            .setSurface(context.getSurface())
            .setMinImageCount(imageCount)
            .setImageFormat(surfaceFormat.format)
            .setImageColorSpace(surfaceFormat.colorSpace)
            .setImageExtent(extent)
            .setImageArrayLayers(1)
            .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst)
            .setImageSharingMode(sameQueueFamily ? vk::SharingMode::eExclusive : vk::SharingMode::eConcurrent)
            .setQueueFamilyIndexCount(sameQueueFamily ? 0u : static_cast<uint32_t>(indices.size()))
            .setPQueueFamilyIndices(sameQueueFamily ? nullptr : indices.data())
            .setPreTransform(details.capabilities.currentTransform)
            .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
            .setPresentMode(presentMode)
            .setClipped(vk::True);
        return swapchainCreateInfo;
    }


}

imp::gfx::SwapChain::SwapChain(const VulkanContext& context, const Window& window) :
    m_supportDetails(context.getPhysicalDevice(), context.getSurface()),
    m_surfaceFormat(chooseSwapSurfaceFormat(m_supportDetails.formats)),
    m_presentMode(chooseSwapPresentMode(m_supportDetails.presentModes, vk::PresentModeKHR::eMailbox)),
    m_extent(chooseSwapExtent(m_supportDetails.capabilities, window)),
    m_swapChain(context.getDevice(), getSwapChainCreateInfo(m_supportDetails, m_surfaceFormat, m_extent, m_presentMode, context)),
    m_images(m_swapChain.getImages()),
    m_imageViews(),
    m_framebuffers(),
    m_presentCompleteSemaphores(),
    m_renderFinishedSemaphores()
{
    createImageViews(context.getDevice());
    for (size_t i = 0; i < m_images.size(); i++) {
        m_presentCompleteSemaphores.emplace_back(context.getDevice(), vk::SemaphoreCreateInfo{});
        m_renderFinishedSemaphores.emplace_back(context.getDevice(), vk::SemaphoreCreateInfo{});
    }
}

void imp::gfx::SwapChain::createSwapChain(const VulkanContext& context, const Window& window)
{
    const auto& device = context.getDevice();
    const vk::SurfaceKHR& surface = context.getSurface();
    m_supportDetails = SwapChainSupportDetails(context.getPhysicalDevice(), surface);
    m_surfaceFormat = chooseSwapSurfaceFormat(m_supportDetails.formats);
    m_presentMode = chooseSwapPresentMode(m_supportDetails.presentModes, vk::PresentModeKHR::eMailbox);
    m_extent = chooseSwapExtent(m_supportDetails.capabilities, window);
    auto createInfo = getSwapChainCreateInfo(m_supportDetails, m_surfaceFormat, m_extent, m_presentMode, context);
    m_swapChain = vk::raii::SwapchainKHR(device, createInfo);
    m_images = m_swapChain.getImages();


}

void imp::gfx::SwapChain::createImageViews(const vk::raii::Device& device)
{
    m_imageViews.clear();
    auto imageViewCreateInfo = vk::ImageViewCreateInfo{}
        .setViewType(vk::ImageViewType::e2D)
        .setFormat(m_surfaceFormat.format)
        .setSubresourceRange(vk::ImageSubresourceRange{}
            .setAspectMask(vk::ImageAspectFlagBits::eColor)
            .setBaseMipLevel(0)
            .setLevelCount(1)
            .setBaseArrayLayer(0)
            .setLayerCount(1));

    m_imageViews.reserve(m_images.size());
    for (auto& image : m_images) {
        imageViewCreateInfo.image = image;
        m_imageViews.emplace_back(device, imageViewCreateInfo);
    }
}

void imp::gfx::SwapChain::cleanup()
{
    m_swapChain = nullptr;
    m_images.clear();
    m_imageViews.clear();
    m_framebuffers.clear();
}

vk::SurfaceFormatKHR imp::gfx::SwapChain::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& formats)
{
    for (const auto& availableFormat : formats) {
        if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return availableFormat;
        }

    }
    Debug::Error("Preferred format not found, using first available format");
    return formats[0];
}
vk::SurfaceFormatKHR imp::gfx::SwapChain::pickSwapSurfaceFormat(const vk::PhysicalDevice device, const vk::SurfaceKHR surface)
{
    try {
        return vkutil::PickSwapchainSurfaceFormat(device, surface);
    }
    catch (const std::runtime_error& e) {
        Debug::FatalError("Failed to pick swap surface format: {}", e.what());
        throw e;
    }
    throw std::runtime_error("Failed to pick swap surface format!");
}
vk::PresentModeKHR imp::gfx::SwapChain::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes, vk::PresentModeKHR preferredMode)
{
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
            return availablePresentMode;
        }
    }
    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D imp::gfx::SwapChain::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, const Window& window)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    auto size = window.getSize();
    auto width = static_cast<uint32_t>(size.first);
    auto height = static_cast<uint32_t>(size.second);
    return {
        std::clamp<uint32_t>(static_cast<uint32_t>(width), capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
        std::clamp<uint32_t>(static_cast<uint32_t>(height), capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
    };
}

void imp::gfx::SwapChain::recreateSwapChain(const VulkanContext& context, const Window& window)
{
    context.getDevice().waitIdle();
    cleanup();
    createSwapChain(context, window);
    createImageViews(context.getDevice());
}


