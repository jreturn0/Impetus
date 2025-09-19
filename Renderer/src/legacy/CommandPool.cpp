//#include "core/CommandPool.h"
//
//Imp::Render::CommandPool::CommandPool(const vk::Device& device, uint32_t familyIndex) : familyIndex(familyIndex)
//{
//	vk::CommandPoolCreateInfo commandPoolInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
//											  familyIndex);
//	pool= device.createCommandPoolUnique(commandPoolInfo);
//}
//
//Imp::Render::CommandPool::CommandPool()
//{
//}
//
//Imp::Render::CommandPool::~CommandPool()
//{
//}
//
//Imp::Render::UniqueCommandPool Imp::Render::CreateUniqueCommandPool(const vk::Device& device, uint32_t familyIndex)
//{
//	return std::unique_ptr<CommandPool>(new CommandPool(device, familyIndex));
//}
