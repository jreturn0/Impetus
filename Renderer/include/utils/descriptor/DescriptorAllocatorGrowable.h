#pragma once
#include "utils/QuickMacros.h"
#include "utils/VKCommon.hpp"

namespace imp::gfx
{
    // Descriptor Allocator that can grow by creating new pools as needed
    class DescriptorAllocatorGrowable
    {
    public:
        struct PoolSizeRatio
        {
            vk::DescriptorType type;
            float ratio;
        };

        DescriptorAllocatorGrowable(const vk::raii::Device& device, uint32_t initialSets, std::vector<PoolSizeRatio> poolRatios);
        DescriptorAllocatorGrowable() = default;
        DescriptorAllocatorGrowable(const DescriptorAllocatorGrowable&) = delete;
        DescriptorAllocatorGrowable& operator=(const DescriptorAllocatorGrowable&) = delete;
        DescriptorAllocatorGrowable(DescriptorAllocatorGrowable&&) = delete;
        DescriptorAllocatorGrowable& operator=(DescriptorAllocatorGrowable&&) = delete;


        // Methods

        void initPool(const vk::raii::Device& device, uint32_t initialSets, std::span<PoolSizeRatio> poolRatios);
        void clearDescriptors();
        void destroyPool(const vk::raii::Device& device);
        vk::raii::DescriptorSet allocate(const vk::raii::Device& device, vk::DescriptorSetLayout layout, void* pNext = nullptr);



    private:
        std::vector<PoolSizeRatio> m_ratios{};
        std::vector<vk::raii::DescriptorPool> m_fullPools{};
        std::vector<vk::raii::DescriptorPool> m_readyPools{};
        uint32_t m_setsPerPool{};

        // Private Methods

        vk::raii::DescriptorPool getPool(const vk::raii::Device& device);
        vk::raii::DescriptorPool  createPool(const vk::raii::Device& device, uint32_t setCount, std::span<PoolSizeRatio> poolRatios);

    };
    using UniqueDescriptorAllocatorGrowable = std::unique_ptr<DescriptorAllocatorGrowable>;
};