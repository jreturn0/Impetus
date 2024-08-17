#pragma once
#include "core/CommandBuffer.h"
#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"

namespace Imp::Render
{
	class GUI
	{
	private:

		vk::UniqueDescriptorPool pool;
	public:
		DISABLE_COPY_AND_MOVE(GUI);

		GUI(class Instance &instance, class Device& device, class SwapChain& swapChain, class Queue& graphicsQueue, class VKWindow& window );
		~GUI();
		void startFrame();
		void render(const vk::CommandBuffer& cmd);
		void endFrame();

	};
}
