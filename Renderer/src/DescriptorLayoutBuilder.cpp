#include "Debug.h"
#include "utils/descriptor/DescriptorLayoutBuilder.h"

void imp::gfx::DescriptorLayoutBuilder::addBinding(uint32_t binding, vk::DescriptorType type)
{
	bindings.emplace_back(binding, type, 1);
}

void imp::gfx::DescriptorLayoutBuilder::clear()
{
	bindings.clear();
}




vk::raii::DescriptorSetLayout imp::gfx::DescriptorLayoutBuilder::build(const vk::raii::Device& device,
	vk::ShaderStageFlags stageFlags, void* pNext, vk::DescriptorSetLayoutCreateFlags flags) 
{
	for (vk::DescriptorSetLayoutBinding& binding : bindings)
		binding.setStageFlags(stageFlags);
	

	vk::DescriptorSetLayoutCreateInfo layoutInfo(flags,  bindings);
	return device.createDescriptorSetLayout(layoutInfo, nullptr);


}
