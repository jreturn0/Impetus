#include "utils/ResourceCache.h"


imp::gfx::ResourceCache::ResourceCache(const vk::raii::Device& device) :
    m_defaultSamplerLinear(device.createSampler(vk::SamplerCreateInfo{ {},vk::Filter::eLinear,vk::Filter::eLinear })),
    m_defaultSamplerNearest(device.createSampler(vk::SamplerCreateInfo{ {},vk::Filter::eNearest,vk::Filter::eNearest }))
{
    std::vector<DescriptorAllocatorGrowable::PoolSizeRatio> sizes = {
        { vk::DescriptorType::eCombinedImageSampler, 3 },
        { vk::DescriptorType::eUniformBuffer, 3 },
        { vk::DescriptorType::eStorageBuffer, 1 } };

    m_materialDescriptorAllocator.initPool(device, 8, sizes);
    m_meshCache.m_resourceNames.emplace_back("");
    m_materialCache.m_resourceNames.emplace_back("default");

}
