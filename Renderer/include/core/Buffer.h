#pragma once
#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"
#include "core/Vma.h"



namespace Imp::Render {
	class Device;
	using UniqueBuffer = std::unique_ptr<class Buffer>;
	using SharedBuffer = std::shared_ptr<class Buffer>;
	class Buffer
	{
	private:
		// const Device& device;
		VmaAllocator* allocator;
		const vk::DeviceSize size;
		//VkBuffer _buffer;
		vk::Buffer buffer;
		VmaAllocation allocation;
		VmaAllocationInfo allocationInfo;
		vk::MemoryPropertyFlags memoryProperties;
		bool defer = false;

		inline static uint32_t idCounter = 0;
		uint32_t id;
		inline static uint32_t allocated = 0;



		friend std::unique_ptr<Buffer> CreateUniqueBuffer(
			VmaAllocator& allocator,
			vk::DeviceSize size,
			vk::BufferUsageFlags usage,
			VmaMemoryUsage memoryUsage);
		friend SharedBuffer CreateSharedBuffer(
			VmaAllocator& allocator,
			vk::DeviceSize size,
			vk::BufferUsageFlags usage,
			VmaMemoryUsage memoryUsage);
		friend Buffer CreateBuffer(
			VmaAllocator& allocator,
			vk::DeviceSize size,
			vk::BufferUsageFlags usage,
			VmaMemoryUsage memoryUsage, bool defer);

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
		const VkBuffer& getBuffer_() const { return buffer; }
		const vk::Buffer& getBuffer() const { return buffer; }
		VmaAllocation getAllocation() const { return allocation; }
		VmaAllocationInfo getAllocationInfo() const { return allocationInfo; }
	};


	SharedBuffer CreateSharedBuffer(
		VmaAllocator& allocator,
		vk::DeviceSize size,
		vk::BufferUsageFlags usage,
		VmaMemoryUsage memoryUsage);

	UniqueBuffer CreateUniqueBuffer(
		VmaAllocator& allocator,
		vk::DeviceSize size,
		vk::BufferUsageFlags usage,
		VmaMemoryUsage memoryUsage);
	//Buffer CreateBuffer(
 //                                   VmaAllocator& allocator,
 //                                   vk::DeviceSize size,
 //                                   vk::BufferUsageFlags usage,
 //                                   VmaMemoryUsage memoryUsage,bool defer = false);
}
