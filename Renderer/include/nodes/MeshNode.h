#pragma once
#include "nodes/Node.h"
#include <memory>
namespace imp::gfx {
	struct Mesh;

	// Scene graph node representing a 3D mesh
	struct MeshNode : public Node
	{
		std::shared_ptr<Mesh> mesh{nullptr};

		// Overrides

		void draw(const glm::mat4& topMatrix, DrawContext& ctx) override;
		void draw(const glm::mat4& topMatrix, DrawContext& ctx, std::shared_ptr<struct Material>& materialOverride) override;
	};
}
