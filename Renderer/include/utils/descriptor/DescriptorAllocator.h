#pragma once
#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"

namespace imp::gfx
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

        DescriptorAllocator(const  vk::Device& device, uint32_t maxSets, std::span<PoolSizeRatio> poolRatios);

        void init_pool(const  vk::Device& device, uint32_t maxSets, std::span<PoolSizeRatio> poolRatios);
        void clear_descriptors(const  vk::Device& device);
        void destroy_pool(const  vk::Device& device);

        vk::UniqueDescriptorSet allocate(const  vk::Device& device, vk::DescriptorSetLayout layout);
    };

    using UniqueDescriptorAllocator = std::unique_ptr<DescriptorAllocator>;
    UniqueDescriptorAllocator CreateUniqueDescriptorAllocator(const  vk::Device& device, uint32_t maxSets, std::span<DescriptorAllocator::PoolSizeRatio> poolRatios);

    UniqueDescriptorAllocator CreateUniqueDescriptorAllocatorEasyBake(const  vk::Device& device);
}
