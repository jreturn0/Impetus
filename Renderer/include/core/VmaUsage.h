#pragma once
#include <vma/vk_mem_alloc.h>

//Small smart wrapper around a VMA allocation
//class VmaResource
//{
//public:
//    VmaResource() = default;
//    VmaResource(VmaAllocator allocator, VmaAllocation allocation) : m_allocator(allocator), m_allocation(allocation) {}
//    ~VmaResource() { destroy(); }
//    // Disable copy
//    VmaResource(const VmaResource&) = delete;
//    VmaResource& operator=(const VmaResource&) = delete;
//    // Enable move
//    VmaResource(VmaResource&& other) noexcept
//        : m_allocator(other.m_allocator), m_allocation(other.m_allocation)
//    {
//        other.m_allocator = nullptr;
//        other.m_allocation = nullptr;
//    }
//    VmaResource& operator=(VmaResource&& other) noexcept
//    {
//        if (this != &other) {
//            destroy();
//            m_allocator = other.m_allocator;
//            m_allocation = other.m_allocation;
//            other.m_allocator = nullptr;
//            other.m_allocation = nullptr;
//        }
//        return *this;
//    }
//    void destroy()
//    {
//        if (m_allocator && m_allocation) {
//            vmaFreeMemory(m_allocator, m_allocation);
//            m_allocator = nullptr;
//            m_allocation = nullptr;
//        }
//    }
//    VmaAllocator getAllocator() const { return m_allocator; }
//    VmaAllocation getAllocation() const { return m_allocation; }
//private:
//    VmaAllocator m_allocator{ nullptr };
//    VmaAllocation m_allocation{ nullptr };
//    VmaAllocationInfo allocationInfo{};
//};