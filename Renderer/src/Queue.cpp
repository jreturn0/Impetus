#include "pch.h"
#include "core/Queue.h"

#include "core/CommandPool.h"
#include "core/CommandBuffer.h"
#include "core/Device.h"

namespace {
	static uint32_t GetFamilyIndex(const Imp::Render::QueueFamilyIndices& indices, const Imp::Render::QueueFamily family)
	{
		switch (family) {
		case Imp::Render::QueueFamily::Present:
			return indices.presentFamily.value_or(UINT32_MAX);
		case Imp::Render::QueueFamily::Transfer:
			return indices.transferFamily.value_or(UINT32_MAX);
		case Imp::Render::QueueFamily::Graphics:
			return indices.graphicsFamily.value_or(UINT32_MAX);
		}
		return UINT32_MAX;
	}
}
Imp::Render::Queue::Queue(const Device& device, QueueFamily family) :
	family(family),
	queue(device.getLogical().getQueue(GetFamilyIndex(device.getIndices(), family), 0))
{}

Imp::Render::Queue::Queue() = default;

Imp::Render::Queue::~Queue() = default;

void Imp::Render::Queue::oneTimeSubmit(const Device& device, const CommandPool& pool,
	std::function<void(vk::CommandBuffer& cmd)>&& function) const
{
	auto fence = device.getLogical().createFenceUnique(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
	auto commandBuffer = pool.createCommandBuffer(device);
	// Ensure command buffer and fence are initialized
	if (!commandBuffer || !fence) {
		Debug::Throw("Command buffer or fence not initialized.");
	}

	// Reset fence
	(device.getLogical().resetFences(fence.get()));

	// Reset command buffer
	vk::CommandBuffer commandBufferHandle = commandBuffer->getBuffer();
	(commandBufferHandle.reset());

	vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
	(commandBufferHandle.begin(beginInfo));

	function(commandBufferHandle);

	(commandBufferHandle.end());

	vk::CommandBufferSubmitInfo submitInfo(commandBufferHandle);

	submit(vk::SubmitInfo2({}, {}, submitInfo), *fence);

	vkutil::CheckResult(device.getLogical().waitForFences(fence.get(), VK_TRUE, UINT64_MAX));
}

bool Imp::Render::Queue::submit(const vk::SubmitInfo2& info, const vk::Fence fence) const
{
	
	if(!vkutil::CheckResult(queue.submit2(1, &info, fence)))
	{
		Debug::Error("Failed to submit to queue");
		return false;
	}
	return true;
}

Imp::Render::UniqueQueue Imp::Render::CreateUniqueQueue(const Device& device, QueueFamily family)
{
	return std::make_unique<Queue>(device, family);
}
