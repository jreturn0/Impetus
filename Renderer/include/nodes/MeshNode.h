#pragma once
#include <memory>
#include "nodes/Node.h"
namespace Imp::Render {
	struct Mesh;

	struct MeshNode : public Node
	{
		void draw(const glm::mat4& topMatrix, DrawContext& ctx) override;

		void draw(const glm::mat4& topMatrix, DrawContext& ctx, std::shared_ptr<struct Material>& materialOverride) override;

	public:
		std::shared_ptr<Mesh> mesh;
	};
}
