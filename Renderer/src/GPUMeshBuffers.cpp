#include "gpudata/GPUMeshBuffers.h"

#include "core/CommandBuffer.h"
#include "core/Device.h"
#include "core/ImmediateCommands.h"
#include "utils/VKUtils.h"
#include "core/Vma.h"
Imp::Render::GPUMeshBuffers::GPUMeshBuffers(const Device& device, const vk::Queue& queue, const ImmediateCommands& transferCommands, VmaAllocator& allocator, std::span<uint32_t> indices, std::span<Vertex> vertices)
    : indexBuffer(CreateUniqueBuffer( allocator,
                                     indices.size() * sizeof(uint32_t),
                                     vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
                                     VMA_MEMORY_USAGE_GPU_ONLY)),
    vertexBuffer(CreateUniqueBuffer( allocator,
                                    vertices.size() * sizeof(Vertex),
                                    vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eShaderDeviceAddress,
                                    VMA_MEMORY_USAGE_GPU_ONLY)),
vertexAddress(device.getLogical().getBufferAddress(vk::BufferDeviceAddressInfo{vertexBuffer->getBuffer()}))
{
  //  std::cout << "Creating mesh buffers" << std::endl;
  //  std::cout << "Index buffer size: " << indices.size()  << std::endl;
  //  std::cout << "Vertex buffer size: " << vertices.size()  << std::endl;
    auto vertexSize = vertices.size() * sizeof(Vertex);
    auto indexSize = indices.size() * sizeof(uint32_t);
    auto totalSize = vertexSize + indexSize; // Total size needed in the staging buffer

    // Create a staging buffer large enough to hold both vertices and indices
    auto stagingBuffer = CreateUniqueBuffer( allocator,
                                            totalSize,
                                            vk::BufferUsageFlagBits::eTransferSrc,
                                            VMA_MEMORY_USAGE_CPU_ONLY);

    void* data = stagingBuffer->getAllocationInfo().pMappedData;
    if (!data) {
        throw std::runtime_error("Failed to map staging buffer memory!");
    }

    // Copy vertices and indices data to the staging buffer
    memcpy(data, vertices.data(), vertexSize);
    memcpy(static_cast<char*>(data) + vertexSize, indices.data(), indexSize);

    // Submit the copy commands
    transferCommands.immediateSubmit(queue, device.getLogical(), [&](vk::CommandBuffer& cmd) {
        vk::BufferCopy vertRegion(0, 0, vertexSize);
        cmd.copyBuffer(stagingBuffer->getBuffer(), vertexBuffer->getBuffer(), 1, &vertRegion);

        vk::BufferCopy indexRegion(vertexSize, 0, indexSize);
        cmd.copyBuffer(stagingBuffer->getBuffer(), indexBuffer->getBuffer(), 1, &indexRegion);
                                     });
}

