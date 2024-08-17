#pragma once
#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"

namespace Imp::Render
{
	using UniqueCommandBufferArray = std::array<vk::UniqueCommandBuffer, vkutil::MAX_FRAMES_IN_FLIGHT>;
	using UniqueCommandBuffer = std::unique_ptr<class CommandBuffer>;
	class Device;
	class CommandPool;
	class CommandBuffer
	{
	private:
		vk::UniqueCommandBuffer buffer;

		//static UniqueCommandBufferArray fillArray(const Device& device, const CommandPool& commandPool);

		CommandBuffer(const Device& device, const CommandPool& commandPool);

		friend UniqueCommandBuffer CreateUniqueCommandBuffer(const Device& device, const CommandPool& commandPool);
	public:
		DISABLE_COPY_AND_MOVE(CommandBuffer);
		CommandBuffer();
		~CommandBuffer();
		//vk::CommandBuffer& operator[](uint32_t index);
		//vk::CommandBuffer& getBuffer() { return commandBuffer.get(); }
		//const UniqueCommandBufferArray& getBuffers() const;
		const vk::CommandBuffer& getBuffer() const { return *buffer; };
		operator vk::CommandBuffer& () { return *buffer; }
		void beginRecording(vk::CommandBufferBeginInfo beginInfo={});
	};
	UniqueCommandBuffer CreateUniqueCommandBuffer(const Device& device, const CommandPool& commandPool);
}
