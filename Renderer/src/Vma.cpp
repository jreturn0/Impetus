#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>
#include "core/Vma.h"
#include "core/Instance.h"
#include "core/Device.h"

Imp::Render::Vma::Vma(const Instance& instance, const Device& device)
{
	VmaAllocatorCreateInfo info = {};
	info.device = device.getLogical();
	info.physicalDevice = device.getPhysical();
	info.instance = instance.getInstance();
	info.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
	vmaCreateAllocator(&info, &vma);
}

Imp::Render::Vma::~Vma()
{
	vmaDestroyAllocator(vma);
}
