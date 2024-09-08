#include "pch.h"

#include "utils/descriptor/DescriptorAllocator.h"

#include "core/Device.h"

Imp::Render::DescriptorAllocator::DescriptorAllocator(const Device& device, uint32_t maxSets,
	std::span<PoolSizeRatio> poolRatios)
{
	init_pool(device, maxSets, poolRatios);
}

void Imp::Render::DescriptorAllocator::init_pool(const Device& device, uint32_t maxSets,
                                                 std::span<PoolSizeRatio> poolRatios)
{
	std::vector<vk::DescriptorPoolSize> poolSizes;
	for (const auto& poolRatio : poolRatios)
	{
		poolSizes.emplace_back(poolRatio.type, static_cast<uint32_t>(maxSets * poolRatio.ratio));
	}

	vk::DescriptorPoolCreateInfo poolInfo(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, maxSets, poolSizes);
	pool = device.getLogical().createDescriptorPoolUnique(poolInfo);

}

void Imp::Render::DescriptorAllocator::clear_descriptors(const Device& device)
{
	device.getLogical().resetDescriptorPool(*pool);
}

void Imp::Render::DescriptorAllocator::destroy_pool(const Device& device)
{
	device.getLogical().destroyDescriptorPool(*pool);
}

vk::UniqueDescriptorSet Imp::Render::DescriptorAllocator::allocate(const Device& device, vk::DescriptorSetLayout layout)
{
	vk::DescriptorSetAllocateInfo allocInfo(*pool, 1, &layout);

	 return std::move(device.getLogical().allocateDescriptorSetsUnique(allocInfo)[0]);
	
}

Imp::Render::UniqueDescriptorAllocator Imp::Render::CreateUniqueDescriptorAllocator(const Device& device,
	uint32_t maxSets, std::span<DescriptorAllocator::PoolSizeRatio> poolRatios)
{
	return std::unique_ptr<DescriptorAllocator>(new DescriptorAllocator(device, maxSets, poolRatios));
}

Imp::Render::UniqueDescriptorAllocator Imp::Render::CreateUniqueDescriptorAllocatorEasyBake(const Device& device)
{
	std::vector<DescriptorAllocator::PoolSizeRatio> size = { {vk::DescriptorType::eStorageImage, 1.0f} };

	return CreateUniqueDescriptorAllocator(device, 10, size);

}
