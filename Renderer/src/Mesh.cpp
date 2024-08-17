#include "geometry/Mesh.h"

Imp::Render::Mesh::Mesh(const std::string& name,
						const Device& device,
						const vk::Queue& queue,
						const ImmediateCommands& transferCommands,
						VmaAllocator& allocator,
						std::span<GeoSurface> surfaces,
						std::span<uint32_t> indices,
						std::span<Vertex> vertices) :
	name(name),
	surfaces( surfaces.begin(),surfaces.end()),
	meshBuffer(std::make_unique<GPUMeshBuffers>(device, queue, transferCommands, allocator, indices, vertices))
{

}
