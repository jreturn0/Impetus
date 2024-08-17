#include "utils/descriptor/DescriptorWriter.h"

void Imp::Render::DescriptorWriter::write_image(int binding, const vk::ImageView& image, const vk::Sampler& sampler,
												vk::ImageLayout layout, vk::DescriptorType type)
{
	vk::DescriptorImageInfo& info = imageInfos.emplace_back(vk::DescriptorImageInfo{ sampler, image, layout });
	vk::WriteDescriptorSet write{ };
	write.setDstBinding(binding);
	write.setDstSet(nullptr);
	write.setDescriptorType(type);
	write.setImageInfo(info);
	writes.push_back(write);
}

void Imp::Render::DescriptorWriter::write_buffer(int binding, const vk::Buffer& buffer, size_t size, size_t offset,
												 vk::DescriptorType type)
{
	vk::DescriptorBufferInfo& info = bufferInfos.emplace_back(vk::DescriptorBufferInfo{buffer, offset, size });
	vk::WriteDescriptorSet write{ };
	write.setDstBinding(binding);
	write.setDstSet(nullptr);
	write.setDescriptorType(type);
	write.setBufferInfo(info);
	writes.push_back(write);
}

void Imp::Render::DescriptorWriter::clear()
{
	imageInfos.clear();
	writes.clear();
	bufferInfos.clear();
}

void Imp::Render::DescriptorWriter::update_set(const vk::Device& device, vk::DescriptorSet& set)
{
	for (auto& write : writes)
	{
		write.setDstSet(set);
	}
	device.updateDescriptorSets(writes,{});

}
