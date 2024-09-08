#include "pch.h"
#include "utils/descriptor/DescriptorAllocatorGrowable.h"

#include "core/Device.h"

vk::UniqueDescriptorPool Imp::Render::DescriptorAllocatorGrowable::get_pool(const vk::Device& device)
{
	//vk::UniqueDescriptorPool newPool;
	if (!readyPools.empty()) {
		auto newPool = std::move(readyPools.back());
		readyPools.pop_back();
		return newPool;
	}
	auto newPool = create_pool(device, setsPerPool, ratios);
	setsPerPool = setsPerPool * 1.5;
	if (setsPerPool > 4092) {
		setsPerPool = 4092;
	}
	return newPool;
}

vk::UniqueDescriptorPool Imp::Render::DescriptorAllocatorGrowable::create_pool(const vk::Device& device, uint32_t setCount,
																			   std::span<PoolSizeRatio> poolRatios)
{
	std::vector<vk::DescriptorPoolSize> poolSizes;
	for (auto& ratio : poolRatios) {
		poolSizes.emplace_back(ratio.type, static_cast<uint32_t>(ratio.ratio * setCount));
	}
	vk::DescriptorPoolCreateInfo poolInfo(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, setCount, poolSizes);
	return device.createDescriptorPoolUnique(poolInfo);
}






Imp::Render::DescriptorAllocatorGrowable::DescriptorAllocatorGrowable(const vk::Device& device, uint32_t initialSets,
	std::span<PoolSizeRatio> poolRatios)
{
		init_pool(device, initialSets, poolRatios);
}

void Imp::Render::DescriptorAllocatorGrowable::init_pool( const vk::Device& device, uint32_t initialSets,
                                                          std::span<PoolSizeRatio> poolRatios)
{
	ratios.clear();
	for (auto& ratio : poolRatios) {
		ratios.push_back(ratio);
	}
	vk::UniqueDescriptorPool newPool = create_pool(device, initialSets, poolRatios);
	setsPerPool = initialSets*1.5;
	readyPools.push_back(std::move(newPool));
}

void Imp::Render::DescriptorAllocatorGrowable::clear_descriptors( const vk::Device& device)
{
	for (auto& pool : readyPools) {
		device.resetDescriptorPool(*pool);
	}
	for (auto& pool : fullPools) {
		device.resetDescriptorPool(*pool);
		readyPools.push_back(std::move(pool));
	}
	fullPools.clear();
}

void Imp::Render::DescriptorAllocatorGrowable::destroy_pool( const vk::Device& device)
{
	fullPools.clear();
	readyPools.clear();
}

vk::UniqueDescriptorSet Imp::Render::DescriptorAllocatorGrowable::allocate(const vk::Device& device,
																		   vk::DescriptorSetLayout layout, void* pNext) 
{
	// Get or create a pool to allocate from
	auto poolToUse = get_pool(device);

	vk::DescriptorSetAllocateInfo allocInfo(poolToUse.get(), 1, &layout);
	allocInfo.pNext = pNext;

	vk::UniqueDescriptorSet descriptorSet;
	try {
		auto descriptorSets = device.allocateDescriptorSetsUnique(allocInfo);
		descriptorSet = std::move(descriptorSets[0]);
	} catch (const vk::OutOfPoolMemoryError& e) {
		fullPools.push_back(std::move(poolToUse));

		// Allocate a new pool and try again
		poolToUse = get_pool(device);
		allocInfo.descriptorPool = poolToUse.get();

		auto descriptorSets = device.allocateDescriptorSetsUnique(allocInfo);
		descriptorSet = std::move(descriptorSets[0]);
	} catch (const vk::FragmentationError& e) {
		fullPools.push_back(std::move(poolToUse));

		// Allocate a new pool and try again
		poolToUse = get_pool(device);
		allocInfo.descriptorPool = poolToUse.get();

		auto descriptorSets = device.allocateDescriptorSetsUnique(allocInfo);
		descriptorSet = std::move(descriptorSets[0]);
	}

	readyPools.push_back(std::move(poolToUse));
	return descriptorSet;
}

vk::DescriptorSet Imp::Render::DescriptorAllocatorGrowable::allocate_(const vk::Device& device,
	vk::DescriptorSetLayout layout, void* pNext)
{
	// Get or create a pool to allocate from
	auto poolToUse = get_pool(device);

	vk::DescriptorSetAllocateInfo allocInfo(poolToUse.get(), 1, &layout);
	allocInfo.pNext = pNext;

	vk::DescriptorSet descriptorSet;
	try {
		auto descriptorSets = device.allocateDescriptorSets(allocInfo);
		descriptorSet = std::move(descriptorSets[0]);
	} catch (const vk::OutOfPoolMemoryError& e) {
		fullPools.push_back(std::move(poolToUse));

		// Allocate a new pool and try again
		poolToUse = get_pool(device);
		allocInfo.descriptorPool = poolToUse.get();

		auto descriptorSets = device.allocateDescriptorSets(allocInfo);
		descriptorSet = std::move(descriptorSets[0]);
	} catch (const vk::FragmentationError& e) {
		fullPools.push_back(std::move(poolToUse));

		// Allocate a new pool and try again
		poolToUse = get_pool(device);
		allocInfo.descriptorPool = poolToUse.get();

		auto descriptorSets = device.allocateDescriptorSets(allocInfo);
		descriptorSet = std::move(descriptorSets[0]);
	}

	readyPools.push_back(std::move(poolToUse));
	return descriptorSet;
}

Imp::Render::UniqueDescriptorAllocatorGrowable Imp::Render::CreateUniqueDescriptorAllocatorGrowableEasyBake(
	const vk::Device& device)
{
	std::vector<DescriptorAllocatorGrowable::PoolSizeRatio> size = { {vk::DescriptorType::eStorageImage, 1.0f} };

	return std::make_unique<DescriptorAllocatorGrowable>(device, 10, size);

}

