#include "pch.h"


#include "core/CommandBuffer.h"
#include "core/Device.h"
#include "core/CommandPool.h"




Imp::Render::CommandBuffer::CommandBuffer(vk::UniqueCommandBuffer& buffer, vk::CommandBufferLevel level): buffer(std::move(buffer)), level(level)
{}

Imp::Render::CommandBuffer::CommandBuffer() = default;

Imp::Render::CommandBuffer::~CommandBuffer() = default;



void Imp::Render::CommandBuffer::beginRecording(const vk::CommandBufferBeginInfo& beginInfo)
{
	buffer->begin(beginInfo);
}


