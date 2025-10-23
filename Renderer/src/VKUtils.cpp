#include "utils/VKUtils.h"



void imp::gfx::vkutil::CopyBuffer(const vk::raii::Device& device, const vk::Queue queue, const vk::CommandPool commandPool, const vk::Buffer srcBuffer, const vk::Buffer dstBuffer, vk::DeviceSize size, vk::DeviceSize srcOffset, vk::DeviceSize dstOffset)
{
    vk::CommandBufferAllocateInfo bufferInfo{ commandPool,vk::CommandBufferLevel::ePrimary, 1 };
    vk::raii::CommandBuffer commandBuffer = std::move(device.allocateCommandBuffers(bufferInfo)[0]);
    commandBuffer.begin({ vk::CommandBufferUsageFlagBits::eOneTimeSubmit });
    vk::BufferCopy copyRegion{ srcOffset, dstOffset, size };
    commandBuffer.copyBuffer(srcBuffer, dstBuffer, copyRegion);
    commandBuffer.end();
    vk::SubmitInfo submitInfo{};
    submitInfo.setCommandBuffers(*commandBuffer);
    vk::raii::Fence fence = device.createFence(vk::FenceCreateInfo{});
    queue.submit(submitInfo, *fence);
    imp::gfx::vkutil::CheckResult(device.waitForFences(*fence, VK_TRUE, UINT64_MAX));
    queue.waitIdle();
}

vk::ImageSubresourceRange imp::gfx::vkutil::CreateImageSubresourceRange(vk::ImageAspectFlags aspectMask)
{
    vk::ImageSubresourceRange subImage{};
    subImage.setAspectMask(aspectMask)
        .setBaseArrayLayer(0)
        .setBaseMipLevel(0)
        .setLayerCount(vk::RemainingArrayLayers)
        .setLevelCount(vk::RemainingMipLevels);
    return subImage;
}

vk::SurfaceFormatKHR imp::gfx::vkutil::PickSwapchainSurfaceFormat(const vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface)
{
    constexpr static std::array preferedCandidates = {
        vk::Format::eB8G8R8A8Srgb,
        vk::Format::eR8G8B8A8Srgb,
        vk::Format::eB8G8R8A8Unorm,
        vk::Format::eR8G8B8A8Unorm,
    };

    constexpr static auto usage =
        vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst |
        vk::ImageUsageFlagBits::eTransferSrc;
    constexpr static auto tiling = vk::ImageTiling::eOptimal;
    constexpr static auto colorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
    constexpr static auto type = vk::ImageType::e2D;

    std::map<uint64_t, vk::Format> formatScoreMap;
    auto availableFormats = physicalDevice.getSurfaceFormatsKHR(surface);
    for (const auto& candidate : preferedCandidates) {
        for (const auto& format : availableFormats) {
            try {
                auto props = physicalDevice.getImageFormatProperties(
                    format.format,
                    type,
                    tiling,
                    usage
                );
                if (format.format == candidate && format.colorSpace == colorSpace) {

                    return format;  // preferred combo found
                }
            }
            catch (...) {
                continue;
            }
        }
    }
    Debug::Warning("Preferred swapchain format not found, falling back to first valid format.");
    // Fallback to first valid format
    for (const auto& format : availableFormats) {
        try {
            auto prop = physicalDevice.getImageFormatProperties(
                format.format,
                type,
                tiling,
                usage
            );
            return format;
        }
        catch (...) {
            continue;
        }
    }
    Debug::FatalError("Failed to find a suitable swapchain format!");
}

vk::Format imp::gfx::vkutil::PickSupportedFormat(const vk::PhysicalDevice& physicalDevice, const std::span<const vk::Format> candidates, const vk::ImageTiling tiling, const vk::FormatFeatureFlags requiredFeatures)
{
    for (vk::Format format : candidates) {
        auto props = physicalDevice.getFormatProperties(format);

        if (tiling == vk::ImageTiling::eLinear &&
            (props.linearTilingFeatures & requiredFeatures) == requiredFeatures) {
            return format;
        }
        else if (tiling == vk::ImageTiling::eOptimal &&
            (props.optimalTilingFeatures & requiredFeatures) == requiredFeatures) {
            return format;
        }
    }
    throw std::runtime_error("No supported format found for the specified criteria.");
}

vk::Format imp::gfx::vkutil::PickDepthFormat(const vk::PhysicalDevice physicalDevice)
{
    constexpr static std::array candidates = {
        vk::Format::eD32Sfloat,
        vk::Format::eD32SfloatS8Uint,
        vk::Format::eD24UnormS8Uint
    };
    constexpr static auto tiling = vk::ImageTiling::eOptimal;
    constexpr static auto requiredFeatures = vk::FormatFeatureFlagBits::eDepthStencilAttachment;
    try {
        return  vkutil::PickSupportedFormat(physicalDevice, candidates, tiling, requiredFeatures);
    }
    catch (const std::runtime_error& e) {
        Debug::FatalError("Failed to pick depth format: {}", e.what());
        throw e;
    }
    throw std::runtime_error("Failed to find a supported depth format!");
}

vk::Format imp::gfx::vkutil::PickDrawImageFormat(const vk::PhysicalDevice physicalDevice) {
    constexpr static std::array candidates = {
        vk::Format::eR16G16B16A16Sfloat,
        vk::Format::eB8G8R8A8Unorm,
        vk::Format::eR8G8B8A8Unorm
    };
    constexpr static auto tiling = vk::ImageTiling::eOptimal;
    constexpr static auto requiredFeatures =
        vk::FormatFeatureFlagBits::eColorAttachment |
        vk::FormatFeatureFlagBits::eStorageImage |
        vk::FormatFeatureFlagBits::eTransferSrc |
        vk::FormatFeatureFlagBits::eTransferDst;
    try {
        return vkutil::PickSupportedFormat(physicalDevice, candidates, tiling, requiredFeatures);
    }
    catch (const std::runtime_error& e) {
        Debug::FatalError("Failed to find a supported draw image format: {}", e.what());
        throw e;
    }
    throw std::runtime_error("Failed to find a supported draw image format!");
}

vk::Format imp::gfx::vkutil::PickSampledImageFormat(const vk::PhysicalDevice physicalDevice) {
    constexpr static std::array candidates = {
        vk::Format::eR8G8B8A8Srgb,
        vk::Format::eB8G8R8A8Srgb,
        vk::Format::eB8G8R8A8Unorm,
        vk::Format::eR8G8B8A8Unorm
    };
    constexpr static auto tiling = vk::ImageTiling::eOptimal;
    constexpr static auto requiredFeatures = vk::FormatFeatureFlagBits::eSampledImage;
    try {
        return vkutil::PickSupportedFormat(physicalDevice, candidates, tiling, requiredFeatures);
    }
    catch (const std::runtime_error& e) {
        Debug::FatalError("Failed to find a supported sampled image format: {}", e.what());
        throw e;
    }
    throw std::runtime_error("Failed to find a supported sampled image format!");
}

vk::SampleCountFlagBits imp::gfx::vkutil::GetMaxUsableSampleCount(const vk::PhysicalDevice physicalDevice)
{
    vk::PhysicalDeviceProperties physicalDeviceProperties = physicalDevice.getProperties();
    vk::SampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
    if (counts & vk::SampleCountFlagBits::e64) { return vk::SampleCountFlagBits::e64; }
    if (counts & vk::SampleCountFlagBits::e32) { return vk::SampleCountFlagBits::e32; }
    if (counts & vk::SampleCountFlagBits::e16) { return vk::SampleCountFlagBits::e16; }
    if (counts & vk::SampleCountFlagBits::e8) { return vk::SampleCountFlagBits::e8; }
    if (counts & vk::SampleCountFlagBits::e4) { return vk::SampleCountFlagBits::e4; }
    if (counts & vk::SampleCountFlagBits::e2) { return vk::SampleCountFlagBits::e2; }

    return vk::SampleCountFlagBits::e1;
}
