#include <memory>
#include "gpudata/GPUMeshBuffers.h"
#include "core/VulkanContext.h"
#include "core/ImmediateCommands.h"
#include "utils/VKUtils.h"

namespace {


}

imp::gfx::GPUMeshBuffers::GPUMeshBuffers(const VulkanContext& context, std::span<uint32_t> indices, std::span<Vertex> vertices) :
    m_indexBuffer(context.getVmaAllocator(),
        indices.size() * sizeof(uint32_t),
        vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
        VMA_MEMORY_USAGE_GPU_ONLY),
    m_vertexBuffer(context.getVmaAllocator(),
        vertices.size() * sizeof(Vertex),
        vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eShaderDeviceAddress,
        VMA_MEMORY_USAGE_GPU_ONLY),
    m_vertexAddress(context.getDevice().getBufferAddress(vk::BufferDeviceAddressInfo{ m_vertexBuffer.getBuffer() }))
{

    auto vertexSize = vertices.size() * sizeof(Vertex);
    auto indexSize = indices.size() * sizeof(uint32_t);
    auto totalSize = vertexSize + indexSize; // Total size needed in the staging buffer
    // Create a staging buffer large enough to hold both vertices and indices
    auto stagingBuffer = Buffer(context.getVmaAllocator(), totalSize, vk::BufferUsageFlagBits::eTransferSrc, VMA_MEMORY_USAGE_CPU_ONLY);
    void* data = stagingBuffer.getAllocationInfo().pMappedData;
    if (!data) {
        Debug::Throw("Staging buffer memory not mapped!");
    }

    // Copy vertices and indices data to the staging buffer
    memcpy(data, vertices.data(), vertexSize);
    memcpy(static_cast<char*>(data) + vertexSize, indices.data(), indexSize);

    // Submit the copy commands
    context.getGraphicsImmediateCommands().submit( [&](vk::CommandBuffer cmd) {
        vk::BufferCopy vertRegion(0, 0, vertexSize);
        cmd.copyBuffer(stagingBuffer.getBuffer(), m_vertexBuffer.getBuffer(), 1, &vertRegion);

        vk::BufferCopy indexRegion(vertexSize, 0, indexSize);
        cmd.copyBuffer(stagingBuffer.getBuffer(), m_indexBuffer.getBuffer(), 1, &indexRegion);
        });


}



//std::pair<imp::gfx::GPUMeshBuffers, std::future<void>> imp::gfx::CreateGPUMeshBuffersAsync(const VulkanContext& context, std::span<uint32_t> indices, std::span<Vertex> vertices)
//{
//    GPUMeshBuffers buffers;
//    auto vertexSize = vertices.size() * sizeof(Vertex);
//    auto indexSize = indices.size() * sizeof(uint32_t);
//    auto totalSize = vertexSize + indexSize; // Total size needed in the staging buffer
//    // Create a staging buffer large enough to hold both vertices and indices
//    auto stagingBuffer = Buffer(context.getVmaAllocator(), totalSize, vk::BufferUsageFlagBits::eTransferSrc, VMA_MEMORY_USAGE_CPU_ONLY);
//    void* data = stagingBuffer.getAllocationInfo().pMappedData;
//    if (!data) {
//        Debug::Throw("Staging buffer memory not mapped!");
//    }
//
//    // Copy vertices and indices data to the staging buffer
//    memcpy(data, vertices.data(), vertexSize);
//    memcpy(static_cast<char*>(data) + vertexSize, indices.data(), indexSize);
//
//    // Submit the copy commands
//    context.getGraphicsImmediateCommands().submit([&](vk::CommandBuffer cmd) {
//        vk::BufferCopy vertRegion(0, 0, vertexSize);
//        cmd.copyBuffer(stagingBuffer.getBuffer(), m_vertexBuffer.getBuffer(), 1, &vertRegion);
//
//        vk::BufferCopy indexRegion(vertexSize, 0, indexSize);
//        cmd.copyBuffer(stagingBuffer.getBuffer(), m_indexBuffer.getBuffer(), 1, &indexRegion);
//        });
//
//    return std::pair<GPUMeshBuffers, std::future<void>>();
//}
