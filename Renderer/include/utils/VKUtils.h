#pragma once
#include "VKCommon.hpp"


namespace imp::gfx {
    class CommandPool;
    // Utility functions for Vulkan operations
    namespace vkutil {

        void CopyBuffer(const vk::raii::Device& device,
            const vk::Queue queue,
            const vk::CommandPool commandPool,
            const vk::Buffer srcBuffer,
            const vk::Buffer dstBuffer,
            vk::DeviceSize size, vk::DeviceSize srcOffset, vk::DeviceSize dstOffset);

        // Default subresource range for an image, covering all mip levels and array layers
        vk::ImageSubresourceRange CreateImageSubresourceRange(vk::ImageAspectFlags aspectMask);

        vk::SurfaceFormatKHR PickSwapchainSurfaceFormat(const vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface);
        vk::Format PickSupportedFormat(const vk::PhysicalDevice& physicalDevice,
            const std::span<const vk::Format> candidates,
            const vk::ImageTiling tiling,
            const vk::FormatFeatureFlags requiredFeatures);

        vk::Format PickDepthFormat(const vk::PhysicalDevice physicalDevice);

        vk::Format PickDrawImageFormat(const vk::PhysicalDevice physicalDevice);

        vk::Format PickSampledImageFormat(const vk::PhysicalDevice physicalDevice);

        vk::SampleCountFlagBits GetMaxUsableSampleCount(const vk::PhysicalDevice physicalDevice);

    }
}