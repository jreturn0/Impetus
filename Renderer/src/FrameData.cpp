#include "core/Buffer.h"
#include "core/FrameData.h"
#include "core/SwapChain.h"
#include "gpudata/GPUSceneData.h"
#include <gpudata/GPULight.h>
#include <utils/descriptor/DescriptorWriter.h>

imp::gfx::FrameData::~FrameData()
{
    flushDeletions();
}

imp::gfx::FrameDataArray imp::gfx::CreateFrameDataArray(const vk::raii::Device& device,const VmaAllocator allocator, const SwapChain& swapChain, const vk::raii::CommandPool& graphicsCommandPool,
    const vk::raii::DescriptorSetLayout& layout)
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
        frame->m_lightDataBuffer = Buffer{ allocator, sizeof(GPULightData) , vk::BufferUsageFlagBits::eStorageBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU };
        frame->m_inFlightFence = device.createFence(fenceInfo);
        std::vector<DescriptorAllocatorGrowable::PoolSizeRatio> frameSizes = {
            {vk::DescriptorType::eStorageImage, 3},
            {vk::DescriptorType::eStorageBuffer, 4},
            {vk::DescriptorType::eUniformBuffer, 3},
            {vk::DescriptorType::eCombinedImageSampler, 4},
        };
        frame->m_descriptorAllocator.initPool(device, 10, frameSizes);
        frame->m_sceneDataDescriptorSet = frame->m_descriptorAllocator.allocate(device, *layout);

        DescriptorWriter writer{};
        // Correct order: size then offset
        writer.writeBuffer(0, frame->m_sceneDataBuffer.getBuffer(), sizeof(GPUSceneData), 0, vk::DescriptorType::eUniformBuffer);
        writer.writeBuffer(1, frame->m_lightDataBuffer.getBuffer(), sizeof(GPULightData), 0, vk::DescriptorType::eStorageBuffer);
        writer.updateSet(device, frame->m_sceneDataDescriptorSet);
    }
    return std::move(fda);
}
