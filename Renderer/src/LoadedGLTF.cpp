#include "utils/LoadedGLTF.h"

std::shared_ptr<imp::gfx::Node> imp::gfx::LoadedGLTF::getNode(std::string_view name) const
{

    auto it = nodes.find(name);
    if (it != nodes.end()) {
        return it->second;
    }
    Debug::Error("Could not find node {}", name);
    return nullptr;
}

imp::gfx::Node& imp::gfx::LoadedGLTF::getNodeRef(std::string_view name) const
{
    auto it = nodes.find(name);
    if (it != nodes.end()) {
        return *it->second;
    }
    Debug::AssertThrow(false, "Could not find node {}", name);
}

void imp::gfx::LoadedGLTF::draw(const glm::mat4& topMatrix, DrawContext& ctx)
{
    // create renderables from the scenenodes
    for (auto& n : topNodes) {
        n->draw(topMatrix, ctx);
    }
}

void imp::gfx::LoadedGLTF::draw(const glm::mat4& topMatrix, DrawContext& ctx, SharedMaterial& materialOverride)
{
    // create renderables from the scenenodes
    for (auto& n : topNodes) {
        n->draw(topMatrix, ctx, materialOverride);
    }
}

void imp::gfx::LoadedGLTF::clearAll()
{


}
