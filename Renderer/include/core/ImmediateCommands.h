#pragma once
#include <functional>

#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"

namespace Imp::Render {
	class ImmediateCommands
	{
	private:
		vk::UniqueFence fence;
		std::unique_ptr< class CommandPool > commandPool;
		std::unique_ptr< class CommandBuffer > commandBuffer;
	public:
		DISABLE_COPY_AND_MOVE(ImmediateCommands);
		ImmediateCommands (const class Device& device,uint32_t familyIndex);
		~ImmediateCommands();
		void immediateSubmit(const vk::Queue& queue, const vk::Device& device, std::function<void(vk::CommandBuffer& cmd)>&& function) const;
	};
}
