#pragma once
#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"

namespace Imp::Render
{
	class DescriptorLayoutBuilder
	{
	private:
		std::vector<vk::DescriptorSetLayoutBinding> bindings;

	public:
		DescriptorLayoutBuilder() = default;
		DISABLE_COPY_AND_MOVE(DescriptorLayoutBuilder);

		void addBinding(uint32_t binding, vk::DescriptorType type);

		void clear();
		const std::vector<vk::DescriptorSetLayoutBinding>& getBindings() const;

		vk::UniqueDescriptorSetLayout build(const vk::Device& device, vk::ShaderStageFlags stageFlags, void* pNext=nullptr, vk::DescriptorSetLayoutCreateFlags={}) ;

	};
}
