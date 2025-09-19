#include "utils/descriptor/DescriptorWriter.h"

void imp::gfx::DescriptorWriter::writeImage(int binding, vk::ImageView image, vk::Sampler sampler,
    vk::ImageLayout layout, vk::DescriptorType type)
{
    vk::DescriptorImageInfo& info = imageInfos.emplace_back(vk::DescriptorImageInfo{ sampler, image, layout });
    vk::WriteDescriptorSet write{ };
    write.setDstBinding(binding)
        .setDstSet(nullptr)
        .setDescriptorType(type)
        .setImageInfo(info);
    writes.push_back(write);
}

void imp::gfx::DescriptorWriter::writeBuffer(int binding, vk::Buffer buffer, size_t size, size_t offset,
    vk::DescriptorType type)
{
    vk::DescriptorBufferInfo& info = bufferInfos.emplace_back(vk::DescriptorBufferInfo{ buffer, offset, size });
    vk::WriteDescriptorSet write{};
    write.setDstBinding(binding)
        .setDstSet(nullptr)
        .setDescriptorType(type)
        .setBufferInfo(info);
    writes.push_back(write);
}

void imp::gfx::DescriptorWriter::clear()
{
    imageInfos.clear();
    writes.clear();
    bufferInfos.clear();
}

void imp::gfx::DescriptorWriter::updateSet(vk::Device device, vk::DescriptorSet set)
{
    for (auto& write : writes)
    {
        write.setDstSet(set);
    }
    device.updateDescriptorSets(writes, {});

}
