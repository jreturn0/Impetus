//#include "core/CommandBuffer.h"
//#include "core/CommandPool.h"
//
//
//Imp::Render::CommandBuffer::CommandBuffer(const vk::Device& device, const CommandPool& commandPool):
//	buffer(std::move(device.allocateCommandBuffersUnique(
//		vk::CommandBufferAllocateInfo{ commandPool.getPool(), vk::CommandBufferLevel::ePrimary, 1 })[0]))
//{}
//
//Imp::Render::CommandBuffer::CommandBuffer() = default;
//
//Imp::Render::CommandBuffer::~CommandBuffer()
//{
//}
//
//
//
//void Imp::Render::CommandBuffer::beginRecording(vk::CommandBufferBeginInfo beginInfo)
//{
//	
//	buffer->begin(beginInfo);
//}
//
//Imp::Render::UniqueCommandBuffer Imp::Render::CreateUniqueCommandBuffer(const vk::Device& device,
//	const CommandPool& commandPool)
//{
//	return std::unique_ptr<CommandBuffer>(new CommandBuffer(device, commandPool));
//}
