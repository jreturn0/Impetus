#pragma once
#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"

namespace imp::gfx
{
    class DescriptorAllocatorGrowable
    {
    public:
        struct PoolSizeRatio
        {
            vk::DescriptorType type;
            float ratio;
        };

    public:
        DescriptorAllocatorGrowable(const vk::raii::Device& device, uint32_t initialSets, std::vector<PoolSizeRatio> poolRatios);
        DescriptorAllocatorGrowable() = default;

        DescriptorAllocatorGrowable(const DescriptorAllocatorGrowable&) = delete;
        DescriptorAllocatorGrowable& operator=(const DescriptorAllocatorGrowable&) = delete;
        DescriptorAllocatorGrowable(DescriptorAllocatorGrowable&&) = delete;
        DescriptorAllocatorGrowable& operator=(DescriptorAllocatorGrowable&&) = delete;



        void initPool(const vk::raii::Device& device, uint32_t initialSets, std::span<PoolSizeRatio> poolRatios);
        void clearDescriptors();
        void destroyPool(const vk::raii::Device& device);

        vk::raii::DescriptorSet  allocate(const vk::raii::Device& device, vk::DescriptorSetLayout layout, void* pNext = nullptr);



    private:
        std::vector<PoolSizeRatio> m_ratios;
        std::vector<vk::raii::DescriptorPool> m_fullPools;
        std::vector<vk::raii::DescriptorPool> m_readyPools;
        uint32_t m_setsPerPool;

        vk::raii::DescriptorPool getPool(const vk::raii::Device& device);
        vk::raii::DescriptorPool  createPool(const vk::raii::Device& device, uint32_t setCount, std::span<PoolSizeRatio> poolRatios);

    };
    using UniqueDescriptorAllocatorGrowable = std::unique_ptr<DescriptorAllocatorGrowable>;
};