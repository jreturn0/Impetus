#pragma once
#include "utils/VKCommon.hpp"
#include "core/VmaUsage.h"



namespace imp::gfx {
    // Wrapper around a Vulkan buffer and its associated VMA allocation
    class Buffer
    {
    public:
        Buffer() = default;
        Buffer( VmaAllocator allocator, vk::DeviceSize size, vk::BufferUsageFlags usage, VmaMemoryUsage memoryUsage);
        Buffer(Buffer&& other) noexcept;
        Buffer& operator=(Buffer&& other) noexcept;
        Buffer(const Buffer&) = delete; 
        Buffer& operator=(const Buffer&) = delete; 
        ~Buffer();

        // Getters

        inline vk::DeviceSize getSize() const noexcept { return m_size; }
        inline vk::Buffer getBuffer() const noexcept { return m_buffer; }
        inline VmaAllocation getAllocation() const noexcept { return m_allocation; }
        inline VmaAllocationInfo getAllocationInfo() const noexcept { return m_allocationInfo; }

        // Methods

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
