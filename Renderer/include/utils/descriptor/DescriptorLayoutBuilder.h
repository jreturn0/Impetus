#pragma once
#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"

namespace imp::gfx
{
	class DescriptorLayoutBuilder
	{

	public:
		DescriptorLayoutBuilder() = default;
		DescriptorLayoutBuilder(const DescriptorLayoutBuilder&) = delete; 
		DescriptorLayoutBuilder& operator=(const DescriptorLayoutBuilder&) = delete; 
		DescriptorLayoutBuilder(DescriptorLayoutBuilder&&) = delete; 
		DescriptorLayoutBuilder& operator=(DescriptorLayoutBuilder&&) = delete;;

		void addBinding(uint32_t binding, vk::DescriptorType type);

		void clear();
		inline const std::vector<vk::DescriptorSetLayoutBinding>& getBindings() const { return bindings; };

		vk::raii::DescriptorSetLayout build(const vk::raii::Device& device, vk::ShaderStageFlags stageFlags, void* pNext=nullptr, vk::DescriptorSetLayoutCreateFlags={}) ;
	private:
		std::vector<vk::DescriptorSetLayoutBinding> bindings;

	};
}
