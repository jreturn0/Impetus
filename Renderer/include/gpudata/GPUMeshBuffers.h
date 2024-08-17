#pragma once

#include "core/Buffer.h"
#include "geometry/Vertex.h"
#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"

namespace Imp::Render
{
	class ImmediateCommands;
	class CommandPool;

    class GPUMeshBuffers
    {
    private:
        UniqueBuffer indexBuffer;
        UniqueBuffer vertexBuffer;
        vk::DeviceAddress vertexAddress;

    public:
        DISABLE_COPY_AND_MOVE(GPUMeshBuffers);

        UniqueBuffer& getIndexBuffer() { return indexBuffer; }
        UniqueBuffer& getVertexBuffer() { return vertexBuffer; }
        vk::DeviceAddress getVertexAddress() const { return vertexAddress; }

        GPUMeshBuffers(const Device& device, const vk::Queue& queue,const ImmediateCommands& transferCommands, VmaAllocator& allocator, std::span<uint32_t> indices, std::span<Vertex> vertices);
        GPUMeshBuffers() = default;
    };


  
}
