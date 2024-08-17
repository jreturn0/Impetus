#include "core/Queue.h"

#include "core/Device.h"


static uint32_t GetFamilyIndex(Imp::Render::QueueFamilyIndices indices, Imp::Render::QueueFamily family)
{
	switch (family)
	{
	case Imp::Render::QueueFamily::Present:
		return indices.presentFamily.value();
	case Imp::Render::QueueFamily::Transfer:
		return indices.transferFamily.value();
	case Imp::Render::QueueFamily::Graphics:
		return indices.graphicsFamily.value();
	}
}

Imp::Render::Queue::Queue(const Device& device, QueueFamily family) :
family(family),
queue(device.getLogical().getQueue(GetFamilyIndex(device.getIndices(),family), 0))
{
}

Imp::Render::Queue::Queue()
{
}

Imp::Render::Queue::~Queue()
{
}

Imp::Render::UniqueQueue Imp::Render::CreateUniqueQueue(const Device& device, QueueFamily family)
{
	return std::unique_ptr<Queue>(new Queue(device,family));
}
