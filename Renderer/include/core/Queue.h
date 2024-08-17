#pragma once
#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"

namespace Imp::Render
{
	class Device;
	using UniqueQueue = std::unique_ptr<class Queue>;
	enum QueueFamily
	{
		Graphics,
		Present,
		Transfer
	};
	class Queue
	{
	private:
		QueueFamily family;
		vk::Queue queue;
		Queue(const Device& device, QueueFamily family);


		friend UniqueQueue CreateUniqueQueue(const Device& device, QueueFamily family);

	public:
		Queue();
		~Queue();
		DISABLE_COPY_AND_MOVE(Queue);
		const vk::Queue& getQueue() const { return queue; }
		operator vk::Queue& () { return queue; }
		operator const vk::Queue& () const { return queue; }
	};

	UniqueQueue CreateUniqueQueue(const Device& device, QueueFamily family);
}
