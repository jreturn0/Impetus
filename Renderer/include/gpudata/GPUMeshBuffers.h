#pragma once
#include "core/Buffer.h"
#include "geometry/Vertex.h"
#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"

namespace imp::gfx
{
    class VulkanContext;



    // Manages GPU buffers for mesh data (vertices and indices)
    class GPUMeshBuffers
    {
       // friend std::pair<GPUMeshBuffers, std::future<void>> CreateGPUMeshBuffersAsync(const VulkanContext& context, std::span<uint32_t> indices, std::span<Vertex> vertices);
    public:
        GPUMeshBuffers( const VulkanContext& context, std::span<uint32_t> indices, std::span<Vertex> vertices);
        GPUMeshBuffers() = default;
        GPUMeshBuffers(const GPUMeshBuffers&) = delete;
        GPUMeshBuffers& operator=(const GPUMeshBuffers&) = delete;
        GPUMeshBuffers(GPUMeshBuffers&&) = delete;
        GPUMeshBuffers& operator=(GPUMeshBuffers&&) = delete;

        // Getters

        inline Buffer& getIndexBuffer() noexcept  { return m_indexBuffer; }
        inline Buffer& getVertexBuffer() noexcept  { return m_vertexBuffer; }
        inline vk::DeviceAddress getVertexAddress() const noexcept { return m_vertexAddress; }
    private:
        Buffer m_indexBuffer{};
        Buffer m_vertexBuffer{};
        vk::DeviceAddress m_vertexAddress{};

    };

   // std::pair<GPUMeshBuffers, std::future<void>> CreateGPUMeshBuffersAsync(const VulkanContext& context, std::span<uint32_t> indices, std::span<Vertex> vertices);


}
