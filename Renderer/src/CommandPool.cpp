#include "core/CommandPool.h"
#include "core/Device.h"
Imp::Render::CommandPool::CommandPool(const Device& device, uint32_t familyIndex) : familyIndex(familyIndex)
{
	vk::CommandPoolCreateInfo commandPoolInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
											  familyIndex);
	pool= device.getLogical().createCommandPoolUnique(commandPoolInfo);
}

Imp::Render::CommandPool::CommandPool()
{
}

Imp::Render::CommandPool::~CommandPool()
{
}

Imp::Render::UniqueCommandPool Imp::Render::CreateUniqueCommandPool(const Device& device, uint32_t familyIndex)
{
	return std::unique_ptr<CommandPool>(new CommandPool(device, familyIndex));
}
