#pragma once
#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"

namespace Imp::Render
{
    class DescriptorAllocator
    {
    private:
        vk::UniqueDescriptorPool pool;
    public:
        struct PoolSizeRatio
        {
            vk::DescriptorType type;
            float ratio;
        };

    	DescriptorAllocator(const class Device& device, uint32_t maxSets, std::span<PoolSizeRatio> poolRatios);

        void init_pool(const class Device& device, uint32_t maxSets, std::span<PoolSizeRatio> poolRatios);
        void clear_descriptors(const class Device& device);
        void destroy_pool(const class Device& device);

        vk::UniqueDescriptorSet allocate(const class Device& device, vk::DescriptorSetLayout layout);
    };

    using UniqueDescriptorAllocator = std::unique_ptr<DescriptorAllocator>;
    UniqueDescriptorAllocator CreateUniqueDescriptorAllocator(const class Device& device, uint32_t maxSets, std::span<DescriptorAllocator::PoolSizeRatio> poolRatios);

    UniqueDescriptorAllocator CreateUniqueDescriptorAllocatorEasyBake(const class Device& device);
}
