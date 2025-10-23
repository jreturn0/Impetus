#include "core/Buffer.h"



imp::gfx::Buffer::Buffer(VmaAllocator allocator, vk::DeviceSize size,
    vk::BufferUsageFlags usage, VmaMemoryUsage memoryUsage)
    : m_allocator(allocator), m_size(size)
{
    VmaAllocationCreateInfo allocCreateInfo = {};
    allocCreateInfo.usage = memoryUsage;
    allocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
    const auto bufferInfo = vk::BufferCreateInfo{}
        .setSize(size)
        .setUsage(usage);
    VkBuffer _buffer;
    if (vmaCreateBuffer(allocator, &*bufferInfo, &allocCreateInfo, &_buffer, &m_allocation, &m_allocationInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }
    m_buffer = vk::Buffer{ _buffer };
}


imp::gfx::Buffer::Buffer(Buffer&& other) noexcept
    : m_allocator(other.m_allocator), m_size(other.m_size), m_buffer(other.m_buffer),
    m_allocation(other.m_allocation), m_allocationInfo(other.m_allocationInfo),
    m_memoryProperties(other.m_memoryProperties)
{
    other.m_buffer = nullptr;
    other.m_allocation = nullptr;
    other.m_size = 0;
}

imp::gfx::Buffer& imp::gfx::Buffer::operator=(Buffer&& other) noexcept
{
    if (this != &other) {
        destroy();
        m_allocator = other.m_allocator;
        m_size = other.m_size;
        m_buffer = other.m_buffer;
        m_allocation = other.m_allocation;
        m_allocationInfo = other.m_allocationInfo;
        m_memoryProperties = other.m_memoryProperties;
        other.m_buffer = nullptr;
        other.m_allocation = nullptr;
        other.m_size = 0;
    }
    return *this;
}

imp::gfx::Buffer::~Buffer()
{
    if (!m_allocator)
    {
        Debug::FatalError("No allocator in buffer destructor");
    }

    destroy();

}

void imp::gfx::Buffer::destroy()
{
    if (m_buffer && m_allocation) {
        vmaDestroyBuffer(m_allocator, m_buffer, m_allocation);
        m_buffer = nullptr;
        m_allocation = nullptr;
    }
}

