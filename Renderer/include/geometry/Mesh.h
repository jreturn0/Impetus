#pragma once
#include <string>
#include "utils/Forwards.h"
#include "gpudata/GPUMeshBuffers.h"
#include "GeoSurface.h"

namespace imp::gfx
{


    // Structure representing a 3D mesh with surfaces and GPU buffers
    struct Mesh
    {
        std::string name{};
        std::vector<GeoSurface> surfaces{};
        std::unique_ptr<GPUMeshBuffers> meshBuffer{nullptr};

        Mesh(std::string_view name, const VulkanContext& context,
             std::span<GeoSurface> surfaces,
             std::span<uint32_t> indices, 
             std::span<Vertex> vertices);
        Mesh() = default;
        Mesh(const Mesh&) = delete; 
        Mesh& operator=(const Mesh&) = delete; 
        Mesh(Mesh&&) = delete; 
        Mesh& operator=(Mesh&&) = delete;
    };
    using SharedMesh = std::shared_ptr<Mesh>;
}
