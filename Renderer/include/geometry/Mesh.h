#pragma once
#include <string>
#include "utils/Forwards.h"
#include "gpudata/GPUMeshBuffers.h"
#include "GeoSurface.h"

namespace imp::gfx
{



	struct Mesh
	{
		Mesh(std::string_view name, const VulkanContext& context,
			 std::span<GeoSurface> surfaces,
			 std::span<uint32_t> indices, 
			 std::span<Vertex> vertices);
		Mesh() = default;
		DISABLE_COPY_AND_MOVE(Mesh);
		std::string name;

		std::vector<GeoSurface> surfaces;
		std::unique_ptr<GPUMeshBuffers> meshBuffer;

	};
	using SharedMesh = std::shared_ptr<Mesh>;
}
