#pragma once
#include <deque>

#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"

namespace imp::gfx
{
	struct DescriptorWriter
	{
		std::deque<vk::DescriptorImageInfo> imageInfos;
		std::deque<vk::DescriptorBufferInfo> bufferInfos;
		std::vector<vk::WriteDescriptorSet> writes;

		void writeImage(int binding,  vk::ImageView image,  vk::Sampler sampler, vk::ImageLayout layout,
		                 vk::DescriptorType type);
		void writeBuffer(int binding, vk::Buffer buffer, size_t size, size_t offset, vk::DescriptorType type);

		void clear();
		void updateSet( vk::Device device, vk::DescriptorSet set);
		
	};
}
