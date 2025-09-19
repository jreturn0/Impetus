#pragma once
#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"
#include "core/VmaUsage.h"
#include <glm/vec2.hpp>

#include "utils/Forwards.h"
#include <unordered_map>



namespace imp::gfx {

    struct Bitmap {
        std::span<std::byte const> bytes{};
        glm::ivec2 size{};
    };

    struct RawImage {
        bool operator ==(RawImage const& rhs) const = default;
        VmaAllocator allocator{nullptr};
        VmaAllocation allocation{nullptr};
        vk::Image image{nullptr};
        vk::Extent2D extent{};
        vk::Format format{};
        uint32_t mipLevels{};
    };

    class Image
    {

    public:
        Image(const VulkanContext& context, vk::Extent2D extent, vk::Format format, vk::ImageUsageFlags usage, vk::ImageAspectFlags aspectFlags, bool mipmap = false);
        ~Image();

        // Disable copy

        Image(const Image&) = delete;
        Image& operator=(const Image&) = delete;

        // Getters

        const vk::Image getImage() const { return m_raw.image; }
        const vk::raii::ImageView& getView()  const { return view; }
        const vk::Extent3D& getExtent3D() const { return vk::Extent3D{ m_raw.extent }; } //TODO: Legacy, remove later
        const vk::Extent2D& getExtent() const { return m_raw.extent; }
        const vk::Format& getFormat() const { return m_raw.format; }

        // Transition image layout
        void transitionImageLayout(const vk::CommandBuffer& commandBuffer, const vk::ImageLayout& oldLayout, const vk::ImageLayout& newLayout);
    private:
        RawImage m_raw;
        vk::raii::ImageView view;
        

        inline static size_t idCounter = 0;
        size_t id = idCounter++;
    };
    // Utility functions
    

    using UniqueImage = std::unique_ptr<Image>;
    // Create a Unique pointer to an image using bitmap data
    UniqueImage CreateSampledImage(const VulkanContext& context,  Bitmap data, vk::Format format, vk::ImageUsageFlags usage,  bool mipmapped = true);
    using SharedImage = std::shared_ptr<Image>;
    // Create a Unique pointer to an image using bitmap data
    SharedImage CreateSampledSharedImage(const VulkanContext& context,  Bitmap data, vk::Format format, vk::ImageUsageFlags usage, bool mipmapped = true);
    // Copy image by blit to another image
    void CopyImageToImage( const vk::CommandBuffer cmd,const vk::Image src,const vk::Extent2D srcExtent, const vk::Image dst,const  vk::Extent2D dstExtent);
    // Functional wrappers
    void CopyImageToImage(const vk::CommandBuffer cmd, Image& src, Image& dst);
    void CopyImageToSwapChain(const vk::CommandBuffer cmd, Image& src, const SwapChain& swapChain, uint32_t imageIndex);
    // Transition image layout
    void TransitionImageLayout(const vk::CommandBuffer commandBuffer, const vk::Image image,  const vk::ImageLayout oldLayout, const vk::ImageLayout newLayout);
}
