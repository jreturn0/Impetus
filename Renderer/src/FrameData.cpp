#include "pch.h"
#include "core/FrameData.h"
#include "core/Device.h"
#include "core/Instance.h"
#include "core/SwapChain.h"
#include "core/CommandBuffer.h"
#include "core/Buffer.h"
#include "core/CommandPool.h"

Imp::Render::FrameData::~FrameData()
{
	flushDeletions();
}

Imp::Render::FrameDataArray Imp::Render::CreateFrameDataArray(const Device& device, const SwapChain& swapChain, const CommandPool& graphicsCommandPool)
{

	FrameDataArray fda;
	vk::SemaphoreCreateInfo semaphoreInfo;
	vk::FenceCreateInfo fenceInfo({ vk::FenceCreateFlagBits::eSignaled });
	for (auto& frame : fda)
	{
		frame = std::make_unique<FrameData>();
		frame->commandBuffer = graphicsCommandPool.createCommandBuffer(device);
		frame->imageAvailable = device.getLogical().createSemaphoreUnique(semaphoreInfo);
		frame->renderFinished = device.getLogical().createSemaphoreUnique(semaphoreInfo);
		frame->inFlightFence = device.getLogical().createFenceUnique(fenceInfo);
		std::vector<DescriptorAllocatorGrowable::PoolSizeRatio> frameSizes = {
			{vk::DescriptorType::eStorageImage, 3},
			{vk::DescriptorType::eStorageBuffer, 3},
			{vk::DescriptorType::eUniformBuffer, 3},
			{vk::DescriptorType::eCombinedImageSampler, 4},
		};
		frame->descriptorAllocator.init_pool(device.getLogical(), 10, frameSizes);
	}
	return std::move(fda);
}
