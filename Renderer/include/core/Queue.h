#pragma once
#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"

namespace Imp::Render
{
	class Device;


	class Queue
	{
	private:
		QueueFamily family;
		vk::Queue queue;

	public:
		explicit Queue(const Device& device, QueueFamily family);
		Queue();
		~Queue();
		DISABLE_COPY_AND_MOVE(Queue);
		[[nodiscard]] const vk::Queue& getQueue() const { return queue; }
		operator vk::Queue& () { return queue; }
		operator const vk::Queue& () const { return queue; }

		void oneTimeSubmit(const Device& device, const class CommandPool& pool, std::function<void(vk::CommandBuffer& cmd)>&& function) const;
		bool submit(const vk::SubmitInfo2& info, const vk::Fence fence = nullptr) const;;
	};
	using UniqueQueue = std::unique_ptr<class Queue>;
	UniqueQueue CreateUniqueQueue(const Device& device, QueueFamily family);
}
