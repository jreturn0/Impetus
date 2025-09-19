//#include "core/Image.h"
//
//#include "Debug.h"
//#include "core/CommandBuffer.h"
//
//#include "core/SwapChain.h"
//
//#include "core/Buffer.h"
//#include "core/ImmediateCommands.h"	
//
//
//namespace {
//    void SubmitTemporaryCommandBuffer(const Imp::Render::VulkanContext& context,
//        std::function<void(vk::CommandBuffer)> func)
//    {
//        const vk::Device& device = context.getDevice();
//        const auto& graphicsQueue = context.getGraphicsQueue();
//        const auto& queueFamilyIndices = context.getQueueFamilyIndices();
//
//        vk::CommandPoolCreateInfo poolInfo{};
//        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
//        poolInfo.flags = vk::CommandPoolCreateFlagBits::eTransient;
//
//        vk::CommandPool commandPool = device.createCommandPool(poolInfo);
//
//        vk::CommandBufferAllocateInfo allocInfo{};
//        allocInfo.commandPool = commandPool;
//        allocInfo.level = vk::CommandBufferLevel::ePrimary;
//        allocInfo.commandBufferCount = 1;
//
//        vk::CommandBuffer commandBuffer;
//        try {
//            auto buffers = device.allocateCommandBuffers(allocInfo);
//            commandBuffer = buffers[0];  // assign to outer variable
//        }
//        catch (const std::exception& excpt) {
//            fmt::println("Failed to allocate command buffer: {}", excpt.what());
//            device.destroyCommandPool(commandPool);
//            return;
//        }
//
//        vk::CommandBufferBeginInfo beginInfo{};
//        beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
//        commandBuffer.begin(beginInfo);
//
//        func(commandBuffer);
//
//        commandBuffer.end();
//
//        vk::SubmitInfo submitInfo{};
//        submitInfo.commandBufferCount = 1;
//        submitInfo.pCommandBuffers = &commandBuffer;
//
//        graphicsQueue.submit(submitInfo, nullptr);
//        graphicsQueue.waitIdle();
//
//        device.freeCommandBuffers(commandPool, commandBuffer);
//        device.destroyCommandPool(commandPool);
//    }
//
//    vk::ImageSubresourceRange ImageSubresourceRange(vk::ImageAspectFlags aspectMask)
//    {
//        vk::ImageSubresourceRange subImage{};
//        subImage.aspectMask = aspectMask;
//        subImage.baseMipLevel = 0;
//        subImage.levelCount = vk::RemainingMipLevels;
//        subImage.baseArrayLayer = 0;
//        subImage.layerCount = vk::RemainingArrayLayers;
//
//        return subImage;
//    }
//    void GenerateMipmaps(vk::CommandBuffer cmd, vk::Image image, vk::Extent2D imageSize)
//    {
//        int mipLevels = int(std::floor(std::log2(std::max(imageSize.width, imageSize.height)))) + 1;
//        for (int mip = 0; mip < mipLevels; mip++) {
//
//            vk::Extent2D halfSize = imageSize;
//            halfSize.width /= 2;
//            halfSize.height /= 2;
//
//            vk::ImageMemoryBarrier2 imageBarrier{ };
//
//            imageBarrier.srcStageMask = vk::PipelineStageFlagBits2::eAllCommands;// VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
//            imageBarrier.srcAccessMask = vk::AccessFlagBits2::eMemoryWrite;//  VK_ACCESS_2_MEMORY_WRITE_BIT;
//            imageBarrier.dstStageMask = vk::PipelineStageFlagBits2::eAllCommands;// VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
//            imageBarrier.dstAccessMask = vk::AccessFlagBits2::eMemoryWrite | vk::AccessFlagBits2::eMemoryRead;// VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;
//
//            imageBarrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
//            imageBarrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
//
//            vk::ImageAspectFlags aspectMask = vk::ImageAspectFlagBits::eColor;
//            imageBarrier.subresourceRange = ImageSubresourceRange(aspectMask);
//            imageBarrier.subresourceRange.levelCount = 1;
//            imageBarrier.subresourceRange.baseMipLevel = mip;
//            imageBarrier.image = image;
//
//            vk::DependencyInfo depInfo{  };
//            depInfo.imageMemoryBarrierCount = 1;
//            depInfo.pImageMemoryBarriers = &imageBarrier;
//
//            cmd.pipelineBarrier2(depInfo);
//
//            if (mip < mipLevels - 1) {
//                vk::ImageBlit2 blitRegion{ };
//
//                blitRegion.srcOffsets[1].x = imageSize.width;
//                blitRegion.srcOffsets[1].y = imageSize.height;
//                blitRegion.srcOffsets[1].z = 1;
//
//                blitRegion.dstOffsets[1].x = halfSize.width;
//                blitRegion.dstOffsets[1].y = halfSize.height;
//                blitRegion.dstOffsets[1].z = 1;
//
//                blitRegion.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;// VK_IMAGE_ASPECT_COLOR_BIT;
//                blitRegion.srcSubresource.baseArrayLayer = 0;
//                blitRegion.srcSubresource.layerCount = 1;
//                blitRegion.srcSubresource.mipLevel = mip;
//
//                blitRegion.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;// VK_IMAGE_ASPECT_COLOR_BIT;
//                blitRegion.dstSubresource.baseArrayLayer = 0;
//                blitRegion.dstSubresource.layerCount = 1;
//                blitRegion.dstSubresource.mipLevel = mip + 1;
//
//                vk::BlitImageInfo2 blitInfo{ };
//                blitInfo.dstImage = image;
//                blitInfo.dstImageLayout = vk::ImageLayout::eTransferDstOptimal; //VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
//                blitInfo.srcImage = image;
//                blitInfo.srcImageLayout = vk::ImageLayout::eTransferSrcOptimal;
//                blitInfo.filter = vk::Filter::eLinear;
//                blitInfo.regionCount = 1;
//                blitInfo.pRegions = &blitRegion;
//
//                cmd.blitImage2(blitInfo);
//
//                imageSize = halfSize;
//            }
//        }
//        Imp::Render::TransitionImageLayout(image, cmd, vk::ImageLayout::eTransferSrcOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
//        // transition all mip levels into the final read_only layout
//    }
//}
//
//
//Imp::Render::Image::Image(const VulkanContext& context, vk::Extent3D extent, vk::Format format,
//    vk::ImageUsageFlags usage, vk::ImageAspectFlags aspectFlags, bool mipmap)
//    : allocator(context.getVmaAllocator()), allocation(nullptr), extent(extent), format(format)
//{
//    //std::cout << "ImageColor format: " << vk::to_string(format) << std::endl;
//    //std::cout << "ImageColor extent: " << extent.width << " " << extent.height << " " << extent.depth << std::endl;
//
//    // Ensure extent is valid
//    if (extent.width == 0 || extent.height == 0 || extent.depth == 0) {
//        Debug::Throw("Invalid image extent");
//    }
//
//    // Calculate mip levels
//    uint32_t mipLevels = mipmap ? static_cast<uint32_t>(std::floor(std::log2(std::max(extent.width, extent.height)))) + 1 : 1;
//
//    // Include necessary usage flags
//    usage |= vk::ImageUsageFlagBits::eTransferDst;
//
//    // Create the image
//    vk::ImageCreateInfo imageInfo{};
//    imageInfo.imageType = vk::ImageType::e2D;
//    imageInfo.extent = extent;
//    imageInfo.mipLevels = mipLevels;
//    imageInfo.arrayLayers = 1;
//    imageInfo.format = format;
//    imageInfo.tiling = vk::ImageTiling::eOptimal;
//    imageInfo.initialLayout = vk::ImageLayout::eUndefined;
//    imageInfo.usage = usage;
//    imageInfo.samples = vk::SampleCountFlagBits::e1;
//    imageInfo.sharingMode = vk::SharingMode::eExclusive;
//
//    VmaAllocationCreateInfo allocInfo{};
//    allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
//    allocInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
//    VkImage rawImage;
//    VkImageCreateInfo rawImageInfo = static_cast<VkImageCreateInfo>(imageInfo);
//
//    vkutil::CheckResult(static_cast<vk::Result>(vmaCreateImage(context.getVmaAllocator(), &rawImageInfo, &allocInfo, &rawImage, &allocation, nullptr)));
//    vk::Device device = *context.getDevice();
//    image =  vk::UniqueImage(rawImage, device);
//
//    // Create the image view
//    vk::ImageViewCreateInfo viewInfo{};
//    viewInfo.image = *image;
//    viewInfo.viewType = vk::ImageViewType::e2D;
//    viewInfo.format = format;
//    viewInfo.subresourceRange.aspectMask = aspectFlags;
//    viewInfo.subresourceRange.baseMipLevel = 0; // Start from base level
//    viewInfo.subresourceRange.levelCount = mipLevels; // Set the correct number of mip levels
//    viewInfo.subresourceRange.baseArrayLayer = 0;
//    viewInfo.subresourceRange.layerCount = 1;
//
//    view = device.createImageViewUnique(viewInfo);
//}
//
//
//
//
//Imp::Render::Image::~Image()
//{
//    //std::cout << "Destroying Image: " << id << std::endl;
//    // The vk::Unique handles and VMA will take care of cleanup
//    if (allocation && allocator) {
//        // Make sure to release the allocation
//        vmaDestroyImage(allocator, image.release(), allocation);
//    }
//}
//
//const vk::Image& Imp::Render::Image::getImage() const
//{
//    return *image;
//}
//
//const vk::Image* Imp::Render::Image::getpImage() const
//{
//    return &*image;
//}
//
//const vk::ImageView& Imp::Render::Image::getView() const
//{
//    return *view;
//}
//
//const vk::Extent3D& Imp::Render::Image::getExtent() const
//{
//    return extent;
//}
//
//const vk::Extent2D& Imp::Render::Image::getExtent2D() const
//{
//    return { extent.width, extent.height };
//}
//
//const vk::Format& Imp::Render::Image::getFormat() const
//{
//    return format;
//}
//
//void Imp::Render::Image::transitionImageLayout(const vk::CommandBuffer& commandBuffer, const vk::ImageLayout& oldLayout, const vk::ImageLayout& newLayout)
//{
//    vk::ImageMemoryBarrier2 imageBarrier{};
//    imageBarrier.setSrcStageMask(vk::PipelineStageFlagBits2::eAllCommands);
//    imageBarrier.setSrcAccessMask(vk::AccessFlagBits2::eMemoryWrite);
//    imageBarrier.setDstStageMask(vk::PipelineStageFlagBits2::eAllCommands);
//    imageBarrier.setDstAccessMask(vk::AccessFlagBits2::eMemoryWrite | vk::AccessFlagBits2::eMemoryRead);
//
//    imageBarrier.setOldLayout(oldLayout);
//    imageBarrier.setNewLayout(newLayout);
//
//    vk::ImageAspectFlags aspectMask = (newLayout == vk::ImageLayout::eDepthAttachmentOptimal) ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor;
//    imageBarrier.setSubresourceRange({ aspectMask, 0, vk::RemainingMipLevels, 0, vk::RemainingArrayLayers });
//    imageBarrier.setImage(*image);
//
//    vk::DependencyInfo depInfo{};
//    depInfo.setImageMemoryBarriers(imageBarrier);
//
//    commandBuffer.pipelineBarrier2(depInfo);
//}
//
//Imp::Render::UniqueImage Imp::Render::CreateDrawImage(const VulkanContext& context, vk::Extent2D extent, vk::Format format)
//{
//    //TODO: FIX the formatting, needs more than one option 
//    return UniqueImage(new Image(context, { extent.width,extent.height,1 },
//        vk::Format::eR16G16B16A16Sfloat,
//        (vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst |
//            vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eColorAttachment),
//        vk::ImageAspectFlagBits::eColor));
//}
//
//Imp::Render::UniqueImage Imp::Render::CreateDepthImage(const VulkanContext& context, vk::Extent2D extent)
//{
//    return UniqueImage(new Image(context, { extent.width,extent.height,1 },
//        vk::Format::eD32Sfloat,
//        (vk::ImageUsageFlagBits::eDepthStencilAttachment),
//        vk::ImageAspectFlagBits::eDepth));
//}
//
//Imp::Render::UniqueImage Imp::Render::CreateImage(const VulkanContext& context, vk::Extent3D size,
//    vk::Format format, vk::ImageUsageFlags usage, bool mipmapped)
//{
//
//    if (mipmapped) {
//        usage |= vk::ImageUsageFlagBits::eTransferSrc;
//    }
//    return UniqueImage(new Image(context, size, format, usage, vk::ImageAspectFlagBits::eColor, mipmapped));
//}
//
//Imp::Render::UniqueImage Imp::Render::CreateImage(const VulkanContext& context, const ImmediateCommands& transferCommands, const vk::Queue& transferQueue, void* data, vk::Extent3D size, vk::Format format, vk::ImageUsageFlags usage, bool mipmapped)
//{
//    if (mipmapped) {
//        usage |= vk::ImageUsageFlagBits::eTransferSrc;
//    }
//    size_t data_size = size.depth * size.width * size.height * 4;
//
//    auto uploadBuffer = CreateUniqueBuffer(context.getVmaAllocator(), data_size, vk::BufferUsageFlagBits::eTransferSrc, VMA_MEMORY_USAGE_CPU_TO_GPU);
//
//    memcpy(uploadBuffer->getAllocationInfo().pMappedData, data, data_size);
//
//    // Include necessary usage flags
//    usage |= vk::ImageUsageFlagBits::eTransferDst;
//
//    auto newImage = CreateImage(context, size, format, usage, mipmapped);
//
//    transferCommands.immediateSubmit(transferQueue, context.getDevice(), [&](const vk::CommandBuffer& cmd) {
//        // Transition image to transfer destination layout
//        TransitionImageLayout(newImage->getImage(), cmd, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
//        vk::BufferImageCopy copyRegion{
//           0, 0, 0,
//           vk::ImageSubresourceLayers{vk::ImageAspectFlagBits::eColor, 0, 0, 1},
//           {}, size
//        };
//        cmd.copyBufferToImage(uploadBuffer->getBuffer(), newImage->getImage(), vk::ImageLayout::eTransferDstOptimal, copyRegion);
//        // Transition image to shader read layout
//        });
//
//
//    SubmitTemporaryCommandBuffer(context, [&](vk::CommandBuffer graphicsCmd) {
//        if (mipmapped)
//            GenerateMipmaps(graphicsCmd, newImage->getImage(), { size.width, size.height });
//        else
//            TransitionImageLayout(newImage->getImage(), graphicsCmd, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
//        });
//    return std::move(newImage);
//}
//
//Imp::Render::SharedImage Imp::Render::CreateSharedImage(const VulkanContext& context, vk::Extent3D size,
//    vk::Format format, vk::ImageUsageFlags usage, bool mipmapped)
//{
//    if (mipmapped) {
//        usage |= vk::ImageUsageFlagBits::eTransferSrc;
//    }
//    return SharedImage(new Image(context, size, format, usage, vk::ImageAspectFlagBits::eColor, mipmapped));
//}
//
//Imp::Render::SharedImage Imp::Render::CreateSharedImage(const VulkanContext& context,
//    const ImmediateCommands& transferCommands, void* data, vk::Extent3D size,
//    vk::Format format, vk::ImageUsageFlags usage, bool mipmapped)
//{
//    if (mipmapped)
//    {
//        usage |= vk::ImageUsageFlagBits::eTransferSrc;
//    }
//
//    size_t data_size = size.depth * size.width * size.height * 4;
//
//    auto uploadBuffer = CreateUniqueBuffer(context.getVmaAllocator(), data_size, vk::BufferUsageFlagBits::eTransferSrc, VMA_MEMORY_USAGE_CPU_TO_GPU);
//
//    memcpy(uploadBuffer->getAllocationInfo().pMappedData, data, data_size);
//
//    // Include necessary usage flags
//    usage |= vk::ImageUsageFlagBits::eTransferDst;
//
//    auto newImage = CreateSharedImage(context, size, format, usage, mipmapped);
//
//    transferCommands.immediateSubmit(context.getGraphicsQueue(), context.getDevice(), [&](const vk::CommandBuffer& cmd) {
//        // Transition image to transfer destination layout
//        TransitionImageLayout(newImage->getImage(), cmd, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
//
//        vk::BufferImageCopy copyRegion{
//           0, 0, 0,
//           vk::ImageSubresourceLayers{vk::ImageAspectFlagBits::eColor, 0, 0, 1},
//           {}, size
//        };
//        cmd.copyBufferToImage(uploadBuffer->getBuffer(), newImage->getImage(), vk::ImageLayout::eTransferDstOptimal, copyRegion);
//
//
//        });
//
//    SubmitTemporaryCommandBuffer(context, [&](vk::CommandBuffer graphicsCmd) {
//        if (mipmapped)
//            GenerateMipmaps(graphicsCmd, newImage->getImage(), { size.width, size.height });
//        else
//            TransitionImageLayout(newImage->getImage(), graphicsCmd, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
//        });
//
//    return (newImage);
//}
//
//
//void Imp::Render::CopyImageToImage(const CommandBuffer& cmd, Image& src, Image& dst)
//{
//    vk::ImageBlit2 blitRegion{};
//    auto srcExtent = src.getExtent();
//    blitRegion.srcOffsets[1].setX(srcExtent.width);
//    blitRegion.srcOffsets[1].setY(srcExtent.height);
//    blitRegion.srcOffsets[1].setZ(1);
//    auto dstExtent = dst.getExtent();
//    blitRegion.dstOffsets[1].setX(dstExtent.width);
//    blitRegion.dstOffsets[1].setY(dstExtent.height);
//    blitRegion.dstOffsets[1].setZ(1);
//    blitRegion.setSrcSubresource({ vk::ImageAspectFlagBits::eColor, 0, 0, 1 });
//    blitRegion.setDstSubresource({ vk::ImageAspectFlagBits::eColor, 0, 0, 1 });
//    vk::BlitImageInfo2 blitInfo{};
//    blitInfo.setSrcImage(src.getImage());
//    blitInfo.setSrcImageLayout(vk::ImageLayout::eTransferSrcOptimal);
//    blitInfo.setDstImage(dst.getImage());
//    blitInfo.setDstImageLayout(vk::ImageLayout::eTransferDstOptimal);
//    blitInfo.setRegions(blitRegion);
//    blitInfo.setFilter(vk::Filter::eLinear);
//    blitInfo.setRegionCount(1);
//
//    cmd.getBuffer().blitImage2(blitInfo);
//}
//
//void Imp::Render::CopyImageToSwapChain(const vk::CommandBuffer& cmd, Image& src, const SwapChain& swapChain, uint32_t imageIndex)
//{
//    vk::ImageBlit2 blitRegion{};
//    auto srcExtent = src.getExtent();
//    blitRegion.srcOffsets[1].setX(srcExtent.width);
//    blitRegion.srcOffsets[1].setY(srcExtent.height);
//    blitRegion.srcOffsets[1].setZ(1);
//    auto dstExtent = swapChain.getExtent();
//    blitRegion.dstOffsets[1].setX(dstExtent.width);
//    blitRegion.dstOffsets[1].setY(dstExtent.height);
//    blitRegion.dstOffsets[1].setZ(1);
//    blitRegion.setSrcSubresource({ vk::ImageAspectFlagBits::eColor, 0, 0, 1 });
//    blitRegion.setDstSubresource({ vk::ImageAspectFlagBits::eColor, 0, 0, 1 });
//    vk::BlitImageInfo2 blitInfo{};
//    blitInfo.setSrcImage(src.getImage());
//    blitInfo.setSrcImageLayout(vk::ImageLayout::eTransferSrcOptimal);
//    blitInfo.setDstImage(swapChain.getImages()[imageIndex]);
//    blitInfo.setDstImageLayout(vk::ImageLayout::eTransferDstOptimal);
//    blitInfo.setRegions(blitRegion);
//    blitInfo.setFilter(vk::Filter::eLinear);
//    blitInfo.setRegionCount(1);
//
//    cmd.blitImage2(blitInfo);
//}
//
//
//void Imp::Render::TransitionImageLayout(const vk::Image& image, const vk::CommandBuffer& commandBuffer,
//    const vk::ImageLayout& oldLayout, const vk::ImageLayout& newLayout)
//{
//    vk::ImageMemoryBarrier2 imageBarrier{};
//    imageBarrier.setSrcStageMask(vk::PipelineStageFlagBits2::eAllCommands);
//    imageBarrier.setSrcAccessMask(vk::AccessFlagBits2::eMemoryWrite);
//    imageBarrier.setDstStageMask(vk::PipelineStageFlagBits2::eAllCommands);
//    imageBarrier.setDstAccessMask(vk::AccessFlagBits2::eMemoryWrite | vk::AccessFlagBits2::eMemoryRead);
//
//    imageBarrier.setOldLayout(oldLayout);
//    imageBarrier.setNewLayout(newLayout);
//
//    vk::ImageAspectFlags aspectMask = (newLayout == vk::ImageLayout::eDepthAttachmentOptimal) ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor;
//    imageBarrier.setSubresourceRange({ aspectMask, 0, vk::RemainingMipLevels, 0, vk::RemainingArrayLayers });
//    imageBarrier.setImage(image);
//
//    vk::DependencyInfo depInfo{};
//    depInfo.setImageMemoryBarriers(imageBarrier);
//
//    commandBuffer.pipelineBarrier2(depInfo);
//}
