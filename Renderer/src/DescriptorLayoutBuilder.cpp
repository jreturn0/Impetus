#include "utils/descriptor/DescriptorLayoutBuilder.h"

#include "Debug.h"

void Imp::Render::DescriptorLayoutBuilder::addBinding(uint32_t binding, vk::DescriptorType type)
{
	bindings.emplace_back(binding, type, 1);
}

void Imp::Render::DescriptorLayoutBuilder::clear()
{
	bindings.clear();
}


const std::vector<vk::DescriptorSetLayoutBinding>& Imp::Render::DescriptorLayoutBuilder::getBindings() const
{
	return bindings;
}

vk::UniqueDescriptorSetLayout Imp::Render::DescriptorLayoutBuilder::build(const vk::Device& device,
	vk::ShaderStageFlags stageFlags, void* pNext, vk::DescriptorSetLayoutCreateFlags flags) 
{
	for (vk::DescriptorSetLayoutBinding& binding : bindings)
	{
		binding.setStageFlags(stageFlags);
		//Debug::Info("Binding: {} Type: {}", binding.binding, vk::to_string(binding.descriptorType));
		//std::cout << "Binding: " << binding.binding << " Type: " << vk::to_string( binding.descriptorType )<< std::endl;
	}

	vk::DescriptorSetLayoutCreateInfo layoutInfo(flags,  bindings);
	return device.createDescriptorSetLayoutUnique(layoutInfo, nullptr);


}
