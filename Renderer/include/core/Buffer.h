#pragma once
#include <source_location>
#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"
#include "core/VmaUsage.h"



namespace imp::gfx {





    // Wrapper around a Vulkan buffer and its associated VMA allocation
    class Buffer
    {
    public:
        Buffer() = default;
        Buffer( VmaAllocator allocator, vk::DeviceSize size, vk::BufferUsageFlags usage, VmaMemoryUsage memoryUsage);
        ~Buffer();

        // Move only
        Buffer(Buffer&& other) noexcept;
        Buffer& operator=(Buffer&& other) noexcept;

        // Disable copy 
        Buffer(const Buffer&) = delete; 
        Buffer& operator=(const Buffer&) = delete; 

        // Getters
        
        vk::DeviceSize getSize() const { return m_size; } 
        vk::Buffer getBuffer() const { return m_buffer; }
        VmaAllocation getAllocation() const { return m_allocation; }
        VmaAllocationInfo getAllocationInfo() const { return m_allocationInfo; }

        
        void destroy();
    private:
        VmaAllocator m_allocator{ nullptr };
        vk::DeviceSize m_size{};
        vk::Buffer m_buffer{ nullptr };
        VmaAllocation m_allocation{ nullptr };
        VmaAllocationInfo m_allocationInfo{};
        vk::MemoryPropertyFlags m_memoryProperties{};
    };



}
