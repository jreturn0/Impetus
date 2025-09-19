#include "utils/descriptor/DescriptorAllocatorGrowable.h"



vk::raii::DescriptorPool  imp::gfx::DescriptorAllocatorGrowable::getPool(const vk::raii::Device& device)
{
    //vk::UniqueDescriptorPool newPool;
    if (!m_readyPools.empty()) {
        vk::raii::DescriptorPool newPool = std::move(m_readyPools.back());
        m_readyPools.pop_back();
        return newPool;
    }
    auto newPool = createPool(device, m_setsPerPool, m_ratios);
    m_setsPerPool = static_cast<uint32_t>(static_cast<float>(m_setsPerPool) * 1.5);
    constexpr static uint32_t maxSetsPerPool = 4092;
    if (m_setsPerPool > maxSetsPerPool) {
        m_setsPerPool = maxSetsPerPool;
    }
    return newPool;
}

vk::raii::DescriptorPool imp::gfx::DescriptorAllocatorGrowable::createPool(const vk::raii::Device& device, uint32_t setCount,
                                                                               std::span<PoolSizeRatio> poolRatios)
{
    std::vector<vk::DescriptorPoolSize> poolSizes;
    for (auto& ratio : poolRatios) {
        poolSizes.emplace_back(ratio.type, static_cast<uint32_t>(ratio.ratio * setCount));
    }
    vk::DescriptorPoolCreateInfo poolInfo(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, setCount, poolSizes);
    return device.createDescriptorPool(poolInfo);
}






imp::gfx::DescriptorAllocatorGrowable::DescriptorAllocatorGrowable(const vk::raii::Device& device, uint32_t initialSets,
    std::vector<PoolSizeRatio> poolRatios)
{
    initPool(device, initialSets, std::span{ poolRatios });
}

void imp::gfx::DescriptorAllocatorGrowable::initPool( const vk::raii::Device& device, uint32_t initialSets,
                                                          std::span<PoolSizeRatio> poolRatios)
{
    m_ratios.clear();
    for (auto& ratio : poolRatios) {
        m_ratios.push_back(ratio);
    }
    vk::raii::DescriptorPool newPool = createPool(device, initialSets, poolRatios);
    m_setsPerPool = static_cast<uint32_t>(static_cast<float>(initialSets) * 1.5);
    m_readyPools.emplace_back(std::move(newPool));
}

void imp::gfx::DescriptorAllocatorGrowable::clearDescriptors()
{
    for (auto& pool : m_readyPools) {
        pool.reset();

    }
    for (auto& pool : m_fullPools) {
        pool.reset();
        m_readyPools.push_back(std::move(pool));
    }
    m_fullPools.clear();
}

void imp::gfx::DescriptorAllocatorGrowable::destroyPool( const vk::raii::Device& device)
{
    m_fullPools.clear();
    m_readyPools.clear();
}

vk::raii::DescriptorSet imp::gfx::DescriptorAllocatorGrowable::allocate(const vk::raii::Device& device,
                                                                           vk::DescriptorSetLayout layout, void* pNext) 
{
    // Get or create a pool to allocate from
    auto poolToUse = getPool(device);

    vk::DescriptorSetAllocateInfo allocInfo(poolToUse, layout, pNext);

    vk::raii::DescriptorSet descriptorSet{ nullptr };
    try {
        auto descriptorSets = device.allocateDescriptorSets(allocInfo);
        descriptorSet = std::move(descriptorSets[0]);
    } catch (vk::OutOfPoolMemoryError e) {
        m_fullPools.emplace_back(std::move(poolToUse));

        // Allocate a new pool and try again
        poolToUse = getPool(device);
        allocInfo.setDescriptorPool(poolToUse);

        auto descriptorSets = device.allocateDescriptorSets(allocInfo);
        descriptorSet = std::move(descriptorSets[0]);
    } catch ( vk::FragmentationError e) {
        m_fullPools.emplace_back(std::move(poolToUse));

        // Allocate a new pool and try again
        poolToUse = getPool(device);
        allocInfo.setDescriptorPool(poolToUse);

        auto descriptorSets = device.allocateDescriptorSets(allocInfo);
        descriptorSet = std::move(descriptorSets[0]);
    }

    m_readyPools.emplace_back(std::move(poolToUse));
    return descriptorSet;
}



