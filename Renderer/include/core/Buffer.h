#pragma once

#include "utils/QuickMacros.h"
#include "utils/VKCommon.hpp"
#include "core/Vma.h"



namespace Imp::Render {
	class Device;
	class Buffer
	{
	private:
		// const Device& device;
		VmaAllocator* allocator;
		const vk::DeviceSize size;

		vk::Buffer buffer;
		VmaAllocation allocation;
		VmaAllocationInfo allocationInfo;
		vk::MemoryPropertyFlags memoryProperties;

	public:
		Buffer(
			VmaAllocator& allocator,
			vk::DeviceSize size,
			vk::BufferUsageFlags usage,
			VmaMemoryUsage memoryUsage, bool defer = false);
		~Buffer();
		DISABLE_COPY_AND_MOVE(Buffer);

		void destroy();
		vk::DeviceSize getSize() const { return size; }
		const vk::Buffer& getBuffer() const { return buffer; }
		VmaAllocation getAllocation() const { return allocation; }
		VmaAllocationInfo getAllocationInfo() const { return allocationInfo; }
		vk::MemoryPropertyFlags getMemoryProperties() const { return memoryProperties; }

		VkBuffer getBuffer_() const { return buffer; }
	};

	using SharedBuffer = std::shared_ptr<Buffer>;
	SharedBuffer CreateSharedBuffer(
		VmaAllocator& allocator,
		vk::DeviceSize size,
		vk::BufferUsageFlags usage,
		VmaMemoryUsage memoryUsage);

	using UniqueBuffer = std::unique_ptr<Buffer>;
	UniqueBuffer CreateUniqueBuffer(
		VmaAllocator& allocator,
		vk::DeviceSize size,
		vk::BufferUsageFlags usage,
		VmaMemoryUsage memoryUsage);
}
