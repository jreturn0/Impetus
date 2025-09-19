#include "core/Image.h"
#include "core/VulkanContext.h"
#include "core/Buffer.h"
#include <Debug.h>
#include "core/ImmediateCommands.h"
#include "core/SwapChain.h"
#include "utils/VKUtils.h"
#include <concepts>
namespace {

    uint32_t CalculateMipLevels(vk::Extent2D extent) {
        return static_cast<uint32_t>(std::floor(std::log2(std::max(extent.width, extent.height)))) + 1;
    }



    imp::gfx::RawImage CreateRawImage(const imp::gfx::VulkanContext& context, vk::Extent2D extent, vk::Format format, vk::ImageUsageFlags usage, uint32_t mipLevels)
    {
        if (extent.width == 0 || extent.height == 0)
            Debug::Throw("Invalid image extent");
        vk::ImageCreateInfo imageInfo{};
        imageInfo.setImageType(vk::ImageType::e2D)
            .setExtent({ extent.width,extent.height,1 })
            .setFormat(format)
            .setUsage(usage)
            .setArrayLayers(1)
            .setMipLevels(mipLevels)
            .setSamples(vk::SampleCountFlagBits::e1)
            .setTiling(vk::ImageTiling::eOptimal)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setSharingMode(vk::SharingMode::eExclusive);

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        allocInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        VkImage rawImage;
        VkImageCreateInfo rawImageInfo = static_cast<VkImageCreateInfo>(imageInfo);
        VmaAllocation allocation;
        imp::gfx::vkutil::CheckResult(static_cast<vk::Result>(vmaCreateImage(context.getVmaAllocator(), &rawImageInfo, &allocInfo, &rawImage, &allocation, nullptr)));

        return { context.getVmaAllocator(), allocation,  vk::Image(rawImage), vk::Extent2D{ extent.width, extent.height }, format, mipLevels };
    }

    vk::raii::ImageView CreateImageView(const vk::raii::Device& device, const vk::Image image, const vk::Format format, const vk::ImageAspectFlags aspectFlags, const uint32_t mipLevels)
    {
        vk::ImageViewCreateInfo viewInfo{};
        viewInfo.setImage(image)
            .setViewType(vk::ImageViewType::e2D)
            .setFormat(format)
            .subresourceRange.setAspectMask(aspectFlags)
            .setBaseMipLevel(0)
            .setLevelCount(mipLevels)
            .setBaseArrayLayer(0)
            .setLayerCount(1);
        return device.createImageView(viewInfo);
    }

    void GenerateMipmaps(const vk::CommandBuffer cmd, vk::Image image, vk::Extent2D imageSize) {
        auto mipLevels = CalculateMipLevels(imageSize);
        vk::ImageAspectFlags aspectMask = vk::ImageAspectFlagBits::eColor;
        vk::Extent2D halfSize{};
        // Track prev layout


        for (uint32_t mip = 0; mip < mipLevels; mip++) {
            halfSize.setWidth(std::max(1u, imageSize.width / 2));
            halfSize.setHeight(std::max(1u, imageSize.height / 2));
            vk::ImageMemoryBarrier2 barrier{};
            barrier.setSrcStageMask(vk::PipelineStageFlagBits2::eAllCommands)
                .setSrcAccessMask(vk::AccessFlagBits2::eMemoryWrite)
                .setDstStageMask(vk::PipelineStageFlagBits2::eAllCommands)
                .setDstAccessMask(vk::AccessFlagBits2::eMemoryWrite | vk::AccessFlagBits2::eMemoryRead)
                .setOldLayout(vk::ImageLayout::eTransferDstOptimal)
                .setNewLayout(vk::ImageLayout::eTransferSrcOptimal)
                .setSubresourceRange(imp::gfx::vkutil::CreateImageSubresourceRange(aspectMask).setLevelCount(1).setBaseMipLevel(mip))
                .setImage(image);
            vk::DependencyInfo depInfo{};
            depInfo.setImageMemoryBarrierCount(1)
                .setPImageMemoryBarriers(&barrier);
            cmd.pipelineBarrier2(depInfo);
            if (mip < mipLevels - 1) {
                vk::ImageBlit2 blitRegion{};


                blitRegion.srcOffsets[1].setX(imageSize.width)
                    .setY(imageSize.height)
                    .setZ(1);


                blitRegion.dstOffsets[1].setX(halfSize.width)
                    .setY(halfSize.height)
                    .setZ(1);

                blitRegion.srcSubresource.setAspectMask(vk::ImageAspectFlagBits::eColor)
                    .setMipLevel(mip)
                    .setBaseArrayLayer(0)
                    .setLayerCount(1);

                blitRegion.dstSubresource.setAspectMask(vk::ImageAspectFlagBits::eColor)
                    .setMipLevel(mip + 1)
                    .setBaseArrayLayer(0)
                    .setLayerCount(1);

                vk::BlitImageInfo2 blitInfo{};
                blitInfo.setSrcImage(image)
                    .setSrcImageLayout(vk::ImageLayout::eTransferSrcOptimal)
                    .setDstImage(image)
                    .setDstImageLayout(vk::ImageLayout::eTransferDstOptimal)
                    .setRegions(blitRegion)
                    .setFilter(vk::Filter::eLinear);

                cmd.blitImage2(blitInfo);
                imageSize = halfSize;
            }
        }
        imp::gfx::TransitionImageLayout(cmd, image, vk::ImageLayout::eTransferSrcOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

    }


    template<typename PtrType>
    PtrType CreateImage(const imp::gfx::VulkanContext& context,  imp::gfx::Bitmap data, vk::Format format, vk::ImageUsageFlags usage, vk::ImageAspectFlags aspectFlags, bool mipmapped)
    {
        auto& transferCommands = context.getGraphicsImmediateCommands();
        if (mipmapped) {
            usage |= vk::ImageUsageFlagBits::eTransferSrc; // Just a contingency if forgotten
        }
        const auto size = glm::uvec2{ data.size };
        const size_t expectedSize = static_cast<size_t>(size.x) * size.y * 4; // assuming RGBA8
        if (data.bytes.size() < expectedSize) {
            Debug::Throw("Bitmap has {} bytes but {} required for extent {}x{}", data.bytes.size(), expectedSize, size.x, size.y);
        }
        auto dataSize = static_cast<size_t>(size.x) * size.y * 4;
        auto uploadBuffer = imp::gfx::Buffer(context.getVmaAllocator(), dataSize, vk::BufferUsageFlagBits::eTransferSrc, VMA_MEMORY_USAGE_CPU_TO_GPU);
        memcpy(uploadBuffer.getAllocationInfo().pMappedData, data.bytes.data(), dataSize);

        usage |= vk::ImageUsageFlagBits::eTransferDst;

        PtrType newImage{ nullptr };

        if constexpr (std::is_same_v<PtrType, imp::gfx::UniqueImage>) {
            newImage = std::make_unique<imp::gfx::Image>(context, vk::Extent2D{ size.x,size.y }, format, usage, aspectFlags, mipmapped);
        }
        else if constexpr (std::is_same_v<PtrType, imp::gfx::SharedImage>) {
            newImage = std::make_shared<imp::gfx::Image>(context, vk::Extent2D{ size.x,size.y }, format, usage, aspectFlags, mipmapped);
        }
        else {
            static_assert("PtrType must be UniqueImage or SharedImage");
        }




        transferCommands.submit(context.getGraphicsQueue(), [&](const vk::CommandBuffer& cmd) {
            // Transition image to transfer destination layout
            imp::gfx::TransitionImageLayout(cmd, newImage->getImage(), vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
            vk::BufferImageCopy copyRegion{};
            copyRegion.setBufferOffset(0)
                .setBufferRowLength(0)
                .setBufferImageHeight(0)
                .setImageSubresource({ aspectFlags, 0, 0, 1 })
                .setImageOffset({ 0,0,0 })
                .setImageExtent({ size.x,size.y,1 });
            cmd.copyBufferToImage(uploadBuffer.getBuffer(), newImage->getImage(), vk::ImageLayout::eTransferDstOptimal, copyRegion);
            if (mipmapped) 
                GenerateMipmaps(cmd, newImage->getImage(), { size.x, size.y });
            else 
                imp::gfx::TransitionImageLayout(cmd, newImage->getImage(), vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
            

            });
        return std::move(newImage);
    }
}

imp::gfx::Image::Image(const VulkanContext& context, vk::Extent2D extent, vk::Format format, vk::ImageUsageFlags usage, vk::ImageAspectFlags aspectFlags, bool mipmap) :
    m_raw(CreateRawImage(context, extent, format, usage, mipmap ? CalculateMipLevels(extent) : 1)),
    view(CreateImageView(context.getDevice(),m_raw.image,format,aspectFlags,m_raw.mipLevels))
{

}
imp::gfx::Image::~Image() {
    if (m_raw.allocation) {
        vmaDestroyImage(m_raw.allocator, VkImage(m_raw.image), m_raw.allocation);
        m_raw.allocation = nullptr;
    }
}

void imp::gfx::Image::transitionImageLayout(const vk::CommandBuffer& cmd, const vk::ImageLayout& oldLayout, const vk::ImageLayout& newLayout)
{
    TransitionImageLayout(cmd, m_raw.image, oldLayout, newLayout);
}

imp::gfx::UniqueImage imp::gfx::CreateSampledImage(const VulkanContext& context, Bitmap data, vk::Format format, vk::ImageUsageFlags usage, bool mipmapped)
{
    return CreateImage<imp::gfx::UniqueImage>(context, data, format, usage, vk::ImageAspectFlagBits::eColor, mipmapped);
}


imp::gfx::SharedImage imp::gfx::CreateSampledSharedImage(const VulkanContext& context, Bitmap data, vk::Format format, vk::ImageUsageFlags usage, bool mipmapped)
{
    return CreateImage<imp::gfx::SharedImage>(context, data, format, usage, vk::ImageAspectFlagBits::eColor, mipmapped);
}

void imp::gfx::CopyImageToImage(const vk::CommandBuffer cmd, const vk::Image src, const vk::Extent2D srcExtent, const vk::Image dst, const  vk::Extent2D dstExtent)
{
    vk::ImageBlit2 blitRegion{};


    blitRegion.srcOffsets[1].setX(srcExtent.width)
        .setY(srcExtent.height)
        .setZ(1);


    blitRegion.dstOffsets[1].setX(dstExtent.width)
        .setY(dstExtent.height)
        .setZ(1);

    blitRegion.srcSubresource.setAspectMask(vk::ImageAspectFlagBits::eColor)
        .setMipLevel(0)
        .setBaseArrayLayer(0)
        .setLayerCount(1);

    blitRegion.dstSubresource.setAspectMask(vk::ImageAspectFlagBits::eColor)
        .setMipLevel(0)
        .setBaseArrayLayer(0)
        .setLayerCount(1);

    vk::BlitImageInfo2 blitInfo{};

    blitInfo.setSrcImage(src)
        .setSrcImageLayout(vk::ImageLayout::eTransferSrcOptimal)
        .setDstImage(dst)
        .setDstImageLayout(vk::ImageLayout::eTransferDstOptimal)
        .setRegions(blitRegion)
        .setFilter(vk::Filter::eLinear);

    cmd.blitImage2(blitInfo);
}

void imp::gfx::CopyImageToImage(const vk::CommandBuffer cmd, Image& src, Image& dst)
{
    CopyImageToImage(cmd, src.getImage(), src.getExtent(), dst.getImage(), dst.getExtent());
}



void imp::gfx::CopyImageToSwapChain(const vk::CommandBuffer cmd, Image& src, const SwapChain& swapChain, uint32_t imageIndex)
{
    CopyImageToImage(cmd, src.getImage(), src.getExtent(), swapChain.getImages()[imageIndex], swapChain.getExtent());
}

void imp::gfx::TransitionImageLayout(const vk::CommandBuffer cmd, const vk::Image image, const vk::ImageLayout oldLayout, const vk::ImageLayout newLayout)
{
    vk::ImageMemoryBarrier2 imageBarrier{};
    imageBarrier.setSrcStageMask(vk::PipelineStageFlagBits2::eAllCommands)
        .setSrcAccessMask(vk::AccessFlagBits2::eMemoryWrite)
        .setDstStageMask(vk::PipelineStageFlagBits2::eAllCommands)
        .setDstAccessMask(vk::AccessFlagBits2::eMemoryWrite | vk::AccessFlagBits2::eMemoryRead)
        .setOldLayout(oldLayout)
        .setNewLayout(newLayout);

    vk::ImageAspectFlags aspectMask = (newLayout == vk::ImageLayout::eDepthAttachmentOptimal) ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor;
    imageBarrier.setSubresourceRange(vkutil::CreateImageSubresourceRange(aspectMask));
    imageBarrier.setImage(image);

    vk::DependencyInfo depInfo{};
    depInfo.setImageMemoryBarriers(imageBarrier);

    cmd.pipelineBarrier2(depInfo);
}
