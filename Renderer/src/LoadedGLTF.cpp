#include "utils/LoadedGLTF.h"

std::shared_ptr<Imp::Render::Node> Imp::Render::LoadedGLTF::getNode(const std::string& name) const
{

	auto it = nodes.find(name);
	if (it != nodes.end()) {
		return it->second;
	}
	fmt::println(stderr, "Could not find node {}", name);
	return nullptr;
}

Imp::Render::Node& Imp::Render::LoadedGLTF::getNodeRef(const std::string& name) const
{
	auto it = nodes.find(name);
	if (it != nodes.end()) {
		return *it->second;
	}
	fmt::println(stderr, "Could not find node {}", name);
	throw std::runtime_error("Could not find node " + name);
}

void Imp::Render::LoadedGLTF::draw(const glm::mat4& topMatrix, DrawContext& ctx)
{
    // create renderables from the scenenodes
    for (auto& n : topNodes) {
        n->draw(topMatrix, ctx);
    }
}

void Imp::Render::LoadedGLTF::draw(const glm::mat4& topMatrix, DrawContext& ctx, SharedMaterial& materialOverride)
{
	// create renderables from the scenenodes
	for (auto& n : topNodes) {
		n->draw(topMatrix, ctx, materialOverride);
	}
}

void Imp::Render::LoadedGLTF::clearAll()
{


}
