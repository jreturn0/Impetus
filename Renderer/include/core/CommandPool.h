#pragma once
#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"

namespace Imp::Render {
	class Device;
	using UniqueCommandPool = std::unique_ptr<class CommandPool>;
	class CommandBuffer;
	using UniqueCommandBuffer = std::unique_ptr<class CommandBuffer>;
	class CommandPool
	{
	private:
		QueueFamily family;
		uint32_t familyIndex;
		vk::UniqueCommandPool pool;
		vk::UniqueFence fence;

	public:
		explicit CommandPool(const Device& device, QueueFamily family, uint32_t familyIndex);
		CommandPool();
		~CommandPool();
		DISABLE_COPY_AND_MOVE(CommandPool);

		operator vk::CommandPool& () { return*pool; }
		const vk::CommandPool& getPool()const { return *pool; }


		UniqueCommandBuffer createCommandBuffer(const Device& device,vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary) const;

	};

	UniqueCommandPool CreateUniqueCommandPool(const Device& device, QueueFamily family, uint32_t familyIndex);
}
