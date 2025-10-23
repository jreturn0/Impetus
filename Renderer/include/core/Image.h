#pragma once
#include "core/VmaUsage.h"
#include "utils/Forwards.h"
#include "utils/QuickMacros.h"
#include "utils/VKCommon.hpp"
#include <glm/vec2.hpp>
#include <unordered_map>



namespace imp::gfx {
    // Simple bitmap structure
    struct Bitmap {
        std::span<std::byte const> bytes{};
        glm::ivec2 size{};
    };

    // Raw image data structure
    struct RawImage {
        bool operator ==(RawImage const& rhs) const = default;
        VmaAllocator allocator{ nullptr };
        VmaAllocation allocation{ nullptr };
        vk::Image image{ nullptr };
        vk::Extent2D extent{};
        vk::Format format{};
        uint32_t mipLevels{};
    };

    // Manages a Vulkan image through VMA
    class Image
    {
    public:
        Image(const VulkanContext& context, vk::Extent2D extent, vk::Format format, vk::ImageUsageFlags usage, vk::ImageAspectFlags aspectFlags, bool mipmap = false);
        ~Image();
        Image(const Image&) = delete;
        Image& operator=(const Image&) = delete;
        Image(Image&&) = default;
        Image& operator=(Image&&) = default;

        // Getters

        inline vk::Image getImage() const noexcept { return m_raw.image; }
        inline const vk::raii::ImageView& getView()  const noexcept { return m_view; }
        inline vk::Extent2D getExtent() const noexcept { return m_raw.extent; }
        inline const vk::Format& getFormat() const noexcept { return m_raw.format; }

        // Methods

        void transitionImageLayout(const vk::CommandBuffer& commandBuffer, const vk::ImageLayout& oldLayout, const vk::ImageLayout& newLayout);
    private:
        RawImage m_raw{};
        vk::raii::ImageView m_view{ nullptr };
    };

    using UniqueImage = std::unique_ptr<Image>;

    // Utility functions
    namespace vkutil
    {
        // Create a Unique pointer to an image using bitmap data
        UniqueImage CreateSampledImage(const VulkanContext& context, Bitmap data, vk::Format format, vk::ImageUsageFlags usage, bool mipmapped = true);
        using SharedImage = std::shared_ptr<Image>;
        // Create a Unique pointer to an image using bitmap data
        SharedImage CreateSampledSharedImage(const VulkanContext& context, Bitmap data, vk::Format format, vk::ImageUsageFlags usage, bool mipmapped = true);
        // Copy image by blit to another image
        void CopyImageToImage(const vk::CommandBuffer cmd, const vk::Image src, const vk::Extent2D srcExtent, const vk::Image dst, const  vk::Extent2D dstExtent);
        // Copy image by blit to another image
        void CopyImageToImage(const vk::CommandBuffer cmd, Image& src, Image& dst);
        // Copy image by blit to SwapChain image
        void CopyImageToSwapChain(const vk::CommandBuffer cmd, Image& src, const SwapChain& swapChain, uint32_t imageIndex);
        // Transition image layout
        void TransitionImageLayout(const vk::CommandBuffer commandBuffer, const vk::Image image, const vk::ImageLayout oldLayout, const vk::ImageLayout newLayout);
    }
}
