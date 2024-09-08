#include "pch.h"
#include "pch.h"

#include "core/Buffer.h"
#include "core/Device.h"

namespace Imp {
    namespace Render {

        Buffer::Buffer( VmaAllocator& allocator, vk::DeviceSize size,
                       vk::BufferUsageFlags usage, VmaMemoryUsage memoryUsage, bool defer)
            :  allocator(&allocator), size(size)
        {
            VmaAllocationCreateInfo allocCreateInfo = {};
            allocCreateInfo.usage = memoryUsage;
            allocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

            VkBufferCreateInfo bufferInfo = { .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
            bufferInfo.pNext=nullptr;
            bufferInfo.size = size;
            bufferInfo.usage = static_cast<VkBufferUsageFlags>(usage);
            VkBuffer _buffer;

            if ( vmaCreateBuffer(allocator, &bufferInfo, &allocCreateInfo, &_buffer, &allocation, &allocationInfo) != VK_SUCCESS) {
                throw std::runtime_error("failed to allocate buffer memory!");
            }
            buffer = vk::Buffer{ _buffer };
        }





        Buffer::~Buffer()
        {
            if(!allocator)
            {
                std::cout << "NO ALLOCATOR IN BUFFER\n";
            }

            if (buffer && allocation) {
                vmaDestroyBuffer(*allocator, buffer, allocation);
                //  std::cout << "Buffer destroyed, buffers left: " << --allocated << "\n";
            }

        }

        SharedBuffer CreateSharedBuffer(VmaAllocator& allocator, 
                                        vk::DeviceSize size, 
                                        vk::BufferUsageFlags usage,
                                        VmaMemoryUsage memoryUsage)
        {
            return std::make_shared<Buffer>(allocator, size, usage, memoryUsage);
        }

        UniqueBuffer CreateUniqueBuffer(VmaAllocator& allocator,
                                                   vk::DeviceSize size,
                                                   vk::BufferUsageFlags usage,
                                                   VmaMemoryUsage memoryUsage)
        {

            return std::make_unique<Buffer>(allocator, size, usage, memoryUsage);
        }


    } // namespace Render
} // namespace Imp
