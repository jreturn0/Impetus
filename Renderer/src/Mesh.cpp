#include "geometry/Mesh.h"

imp::gfx::Mesh::Mesh(std::string_view name,
    const VulkanContext& context,
    std::span<GeoSurface> surfaces,
    std::span<uint32_t> indices,
    std::span<Vertex> vertices) :
    name(name),
    surfaces(surfaces.begin(), surfaces.end()),
    meshBuffer(std::make_unique<GPUMeshBuffers>(context, indices, vertices))
{

}
