#include "utils/descriptor/DescriptorAllocator.h"



imp::gfx::DescriptorAllocator::DescriptorAllocator(const vk::Device& device, uint32_t maxSets,
    std::span<PoolSizeRatio> poolRatios)
{
    init_pool(device, maxSets, poolRatios);
}

void imp::gfx::DescriptorAllocator::init_pool(const vk::Device& device, uint32_t maxSets,
                                                 std::span<PoolSizeRatio> poolRatios)
{
    std::vector<vk::DescriptorPoolSize> poolSizes;
    for (const auto& poolRatio : poolRatios)
    {
        poolSizes.emplace_back(poolRatio.type, static_cast<uint32_t>(maxSets * poolRatio.ratio));
    }

    vk::DescriptorPoolCreateInfo poolInfo(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, maxSets, poolSizes);
    pool = device.createDescriptorPoolUnique(poolInfo);

}

void imp::gfx::DescriptorAllocator::clear_descriptors(const vk::Device& device)
{
    device.resetDescriptorPool(*pool);
}

void imp::gfx::DescriptorAllocator::destroy_pool(const vk::Device& device)
{
    device.destroyDescriptorPool(*pool);
}

vk::UniqueDescriptorSet imp::gfx::DescriptorAllocator::allocate(const vk::Device& device, vk::DescriptorSetLayout layout)
{
    vk::DescriptorSetAllocateInfo allocInfo(*pool, 1, &layout);

     return std::move(device.allocateDescriptorSetsUnique(allocInfo)[0]);
    
}

imp::gfx::UniqueDescriptorAllocator imp::gfx::CreateUniqueDescriptorAllocator(const vk::Device& device,
    uint32_t maxSets, std::span<DescriptorAllocator::PoolSizeRatio> poolRatios)
{
    return std::unique_ptr<DescriptorAllocator>(new DescriptorAllocator(device, maxSets, poolRatios));
}

imp::gfx::UniqueDescriptorAllocator imp::gfx::CreateUniqueDescriptorAllocatorEasyBake(const vk::Device& device)
{
    std::vector<DescriptorAllocator::PoolSizeRatio> size = { {vk::DescriptorType::eStorageImage, 1.0f} };

    return CreateUniqueDescriptorAllocator(device, 10, size);

}
