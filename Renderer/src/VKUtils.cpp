#include "pch.h"
#include "utils/VKUtils.h"

#include "core/CommandBuffer.h"
#include "core/CommandPool.h"
#include "core/Device.h"
void Imp::Render::vkutil::CopyBuffer(const Device& device, const vk::Queue& queue,
                                     const CommandPool& commandPool, const vk::Buffer& srcBuffer, const vk::Buffer& dstBuffer, vk::DeviceSize size, vk::DeviceSize srcOffset, vk::DeviceSize dstOffset)
{
    auto commandBuffer = commandPool.createCommandBuffer(device, vk::CommandBufferLevel::ePrimary);
    auto commandBuf = commandBuffer.get()->getBuffer();

    commandBuf.begin({ vk::CommandBufferUsageFlagBits::eOneTimeSubmit });

    vk::BufferCopy copyRegion(srcOffset, dstOffset, size);
    commandBuf.copyBuffer(srcBuffer, dstBuffer, 1, &copyRegion);

    commandBuf.end();

    vk::SubmitInfo submitInfo{};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuf;
    vk::UniqueFence fence = device.getLogical().createFenceUnique(vk::FenceCreateInfo());
    queue.submit(submitInfo, *fence);
    CheckResult(device.getLogical().waitForFences(*fence, VK_TRUE, UINT64_MAX));

    queue.waitIdle();
}


