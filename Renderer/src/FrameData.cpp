#include "core/FrameData.h"
#include "core/SwapChain.h"
#include "core/Buffer.h"
#include "gpudata/GPUSceneData.h"

imp::gfx::FrameData::~FrameData()
{
    flushDeletions();



}

imp::gfx::FrameDataArray imp::gfx::CreateFrameDataArray(const vk::raii::Device& device,const VmaAllocator allocator, const SwapChain& swapChain, const vk::raii::CommandPool& graphicsCommandPool)
{

    FrameDataArray fda{};
    vk::SemaphoreCreateInfo semaphoreInfo{};
    vk::FenceCreateInfo fenceInfo({ vk::FenceCreateFlagBits::eSignaled });
    for (auto& frame : fda)
    {
        frame = std::make_unique<FrameData>();
        vk::CommandBufferAllocateInfo allocInfo{ *graphicsCommandPool, vk::CommandBufferLevel::ePrimary, 1 };
        frame->m_commandBuffer = std::move(device.allocateCommandBuffers(allocInfo)[0]); 
        frame->m_sceneDataBuffer = Buffer{ allocator, sizeof(GPUSceneData), vk::BufferUsageFlagBits::eUniformBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU };
        frame->m_imageAvailable = device.createSemaphore(semaphoreInfo);
        frame->m_renderFinished = device.createSemaphore(semaphoreInfo);
        frame->m_inFlightFence = device.createFence(fenceInfo);
        std::vector<DescriptorAllocatorGrowable::PoolSizeRatio> frameSizes = {
            {vk::DescriptorType::eStorageImage, 3},
            {vk::DescriptorType::eStorageBuffer, 3},
            {vk::DescriptorType::eUniformBuffer, 3},
            {vk::DescriptorType::eCombinedImageSampler, 4},
        };
        frame->m_descriptorAllocator.initPool(device, 10, frameSizes);
    }
    return std::move(fda);
}
