#include "core/Buffer.h"
#include "core/Device.h"

namespace Imp {
    namespace Render {

        Buffer::Buffer( VmaAllocator& allocator, vk::DeviceSize size,
                       vk::BufferUsageFlags usage, VmaMemoryUsage memoryUsage, bool defer)
            :  allocator(&allocator), size(size), defer(defer), id(++idCounter)
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
            ++allocated;
        }





        Buffer::~Buffer()
        {
            if(!allocator)
            {
                fmt::println("NO ALLOCATOR IN BUFFER");
            }
            if (!defer) {
                destroy();
            }
            else
            {
              //  std::cout << std::format("Buffer: {}, deferred destruction\n", id);
            }
        }

        void Buffer::destroy()
        {
            if (buffer && allocation ) {
                vmaDestroyBuffer(*allocator, buffer, allocation);
              //  std::cout << "Buffer destroyed, buffers left: " << --allocated << "\n";
            }
        }


        SharedBuffer CreateSharedBuffer(VmaAllocator& allocator, vk::DeviceSize size, vk::BufferUsageFlags usage, VmaMemoryUsage memoryUsage)
        {
            return std::shared_ptr<Buffer>(new Buffer(allocator, size, usage, memoryUsage));
        }

        std::unique_ptr<Buffer> CreateUniqueBuffer(
                                                   VmaAllocator& allocator,
                                                   vk::DeviceSize size,
                                                   vk::BufferUsageFlags usage,
                                                   VmaMemoryUsage memoryUsage)
        {

            return std::unique_ptr<Buffer>(new Buffer( allocator, size, usage, memoryUsage));
        }
        Buffer CreateBuffer(
            VmaAllocator& allocator,
            vk::DeviceSize size,
            vk::BufferUsageFlags usage,
            VmaMemoryUsage memoryUsage, bool defer)
        {

            return { allocator, size, usage, memoryUsage,defer };
        }

    } // namespace Render
} // namespace Imp
