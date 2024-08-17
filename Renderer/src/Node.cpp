#include "nodes/Node.h"

void Imp::Render::Node::refreshTransform(const glm::mat4& parentMatrix)
{
	worldTransform = parentMatrix * localTransform;
	for (auto c : children) {
		c->refreshTransform(worldTransform);
	}
}

void Imp::Render::Node::draw(const glm::mat4& topMatrix, DrawContext& ctx)
{
	// draw children
	for (const auto& c : children) {
		c->draw(topMatrix, ctx);
	}
}

void Imp::Render::Node::draw(const glm::mat4& topMatrix, DrawContext& ctx, std::shared_ptr<struct Material>& materialOverride)
{
	// draw children
	for (const auto& c : children) {
		c->draw(topMatrix, ctx,materialOverride);
	}
}
