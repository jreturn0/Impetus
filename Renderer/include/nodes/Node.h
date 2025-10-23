#pragma once
#include "nodes/IRenderable.h"
#include <glm/mat4x4.hpp>
#include <memory>
#include <vector>


namespace imp::gfx {
    
    // Scene graph node structure for hierarchical transformations and rendering
    struct Node : public IRenderable
    {
        std::weak_ptr<Node> parent{};
        std::vector<std::shared_ptr<Node>> children{};
        glm::mat4 localTransform{};
        glm::mat4 worldTransform{};

        // Methods

        void refreshTransform(const glm::mat4& parentMatrix);

        // Overrides

        void draw(const glm::mat4& topMatrix, DrawContext& ctx) override;
        void draw(const glm::mat4& topMatrix, DrawContext& ctx,
            std::shared_ptr<Material>& materialOverride) override;
    };
}
