#include "nodes/MeshNode.h"
#include <glm/mat4x4.hpp>

#include "geometry/Mesh.h"
#include "materials/Material.h"
#include "rendercontext/DrawContext.h"
#include "rendercontext/RenderObject.h"



void Imp::Render::MeshNode::draw(const glm::mat4& topMatrix, DrawContext& ctx)
{
	const glm::mat4 nodeMatrix = topMatrix * worldTransform;
	//std::cout << "Drawing: " << mesh->name << "\n";
	for (auto& s : mesh->surfaces) {

		RenderObject def{s.count,s.startIndex,mesh->meshBuffer->getIndexBuffer()->getBuffer(),s.material,s.bounds,nodeMatrix,mesh->meshBuffer->getVertexAddress()};
		if (s.material->passType== MaterialPass::Transparent) {
			ctx.transparent.push_back(def);
		} else {
			ctx.opaque.push_back(def);
		}
	}

	// recurse down
	Node::draw(topMatrix, ctx);
}

void Imp::Render::MeshNode::draw(const glm::mat4& topMatrix, DrawContext& ctx,
	std::shared_ptr<Material>& materialOverride)
{
	const glm::mat4 nodeMatrix = topMatrix * worldTransform;
	//std::cout << "Drawing: " << mesh->name << "\n";
	for (auto& s : mesh->surfaces) {

		RenderObject def{ s.count,s.startIndex,mesh->meshBuffer->getIndexBuffer()->getBuffer(),materialOverride,s.bounds,nodeMatrix,mesh->meshBuffer->getVertexAddress() };
		if (materialOverride->passType == MaterialPass::Transparent) {
			ctx.transparent.push_back(def);
		} else {
			ctx.opaque.push_back(def);
		}
	}

	// recurse down
	Node::draw(topMatrix, ctx);
}
