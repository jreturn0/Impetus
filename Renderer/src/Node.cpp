#include "nodes/Node.h"

void imp::gfx::Node::refreshTransform(const glm::mat4& parentMatrix)
{
	worldTransform = parentMatrix * localTransform;
	for (auto c : children) {
		c->refreshTransform(worldTransform);
	}
}

void imp::gfx::Node::draw(const glm::mat4& topMatrix, DrawContext& ctx)
{
	// draw children
	for (const auto& c : children) {
		c->draw(topMatrix, ctx);
	}
}

void imp::gfx::Node::draw(const glm::mat4& topMatrix, DrawContext& ctx, std::shared_ptr<Material>& materialOverride)
{
	// draw children
	for (const auto& c : children) {
		c->draw(topMatrix, ctx,materialOverride);
	}
}
