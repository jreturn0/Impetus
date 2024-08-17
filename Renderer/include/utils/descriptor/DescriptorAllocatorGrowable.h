#pragma once
#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"

namespace Imp::Render
{
	class DescriptorAllocatorGrowable
	{
	public:
		struct PoolSizeRatio
		{
			vk::DescriptorType type;
			float ratio;
		};
	private:
		std::vector<PoolSizeRatio> ratios;
		std::vector<vk::UniqueDescriptorPool> fullPools;
		std::vector<vk::UniqueDescriptorPool> readyPools;
		uint32_t setsPerPool;
		vk::UniqueDescriptorPool get_pool(const vk::Device& device);
		vk::UniqueDescriptorPool create_pool(const vk::Device& device, uint32_t setCount, std::span<PoolSizeRatio> poolRatios);

	public:
		DISABLE_COPY_AND_MOVE(DescriptorAllocatorGrowable);


		DescriptorAllocatorGrowable(const vk::Device& device, uint32_t initialSets, std::span<PoolSizeRatio> poolRatios);
		DescriptorAllocatorGrowable() = default;
		void init_pool( const vk::Device& device, uint32_t initialSets, std::span<PoolSizeRatio> poolRatios);
		void clear_descriptors( const vk::Device& device);
		void destroy_pool( const vk::Device& device);

		vk::UniqueDescriptorSet allocate( const vk::Device& device, vk::DescriptorSetLayout layout,void* pNext=nullptr);
		vk::DescriptorSet allocate_(const vk::Device& device, vk::DescriptorSetLayout layout, void* pNext = nullptr);
	};
	using UniqueDescriptorAllocatorGrowable = std::unique_ptr<DescriptorAllocatorGrowable>;
	UniqueDescriptorAllocatorGrowable CreateUniqueDescriptorAllocatorGrowableEasyBake(const vk::Device& device);
}
