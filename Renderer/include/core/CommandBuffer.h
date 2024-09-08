#pragma once
#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"

namespace Imp::Render
{
	using UniqueCommandBufferArray = std::array<vk::UniqueCommandBuffer, vkutil::MAX_FRAMES_IN_FLIGHT>;
	using UniqueCommandBuffer = std::unique_ptr<class CommandBuffer>;
	class Device;

	class CommandBuffer
	{
	private:
		vk::UniqueCommandBuffer buffer;
		vk::CommandBufferLevel level;
		friend class CommandPool;

		explicit CommandBuffer ( vk::UniqueCommandBuffer& buffer, vk::CommandBufferLevel level);

	public:
		CommandBuffer();
		~CommandBuffer();
		DISABLE_COPY_AND_MOVE(CommandBuffer);


		const vk::CommandBuffer& getBuffer() const { return *buffer; }
		auto getLevel() const { return level; }
		operator vk::CommandBuffer& () { return *buffer; }



		void beginRecording(const vk::CommandBufferBeginInfo& beginInfo={});
	};

}
