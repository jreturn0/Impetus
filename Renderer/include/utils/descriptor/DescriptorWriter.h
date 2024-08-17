#pragma once
#include <deque>

#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"

namespace Imp::Render
{
	struct DescriptorWriter
	{
		std::deque<vk::DescriptorImageInfo> imageInfos;
		std::deque<vk::DescriptorBufferInfo> bufferInfos;
		std::vector<vk::WriteDescriptorSet> writes;

		void write_image(int binding, const vk::ImageView& image, const vk::Sampler& sampler, vk::ImageLayout layout,
		                 vk::DescriptorType type);
		void write_buffer(int binding, const vk::Buffer& buffer, size_t size, size_t offset, vk::DescriptorType type);

		void clear();
		void update_set(const vk::Device& device, vk::DescriptorSet& set);
		
	};
}
