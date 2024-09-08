#include "pch.h"
#include "core/CommandPool.h"

#include "core/CommandBuffer.h"
#include "core/Device.h"
Imp::Render::CommandPool::CommandPool(const Device& device, QueueFamily family, uint32_t familyIndex) :family(family), familyIndex(familyIndex),
	pool(device.getLogical().createCommandPoolUnique({vk::CommandPoolCreateFlagBits::eResetCommandBuffer,familyIndex}))
{

}

Imp::Render::CommandPool::CommandPool() = default;

Imp::Render::CommandPool::~CommandPool() = default;

Imp::Render::UniqueCommandBuffer Imp::Render::CommandPool::createCommandBuffer(const Device& device, vk::CommandBufferLevel level) const
{
	return std::unique_ptr<CommandBuffer>(new CommandBuffer(device.getLogical().allocateCommandBuffersUnique(vk::CommandBufferAllocateInfo{ *pool, level, 1 })[0],level));
}



Imp::Render::UniqueCommandPool Imp::Render::CreateUniqueCommandPool(const Device& device, QueueFamily family, uint32_t familyIndex)
{
	return std::make_unique<CommandPool>(device, family, familyIndex);
}
