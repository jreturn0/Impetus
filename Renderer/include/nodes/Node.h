#pragma once
#include <memory>
#include <vector>
#include <glm/mat4x4.hpp>

#include "nodes/IRenderable.h"
//#include "utils/Forwards.h"

namespace imp::gfx {
    

    struct Node : public IRenderable
    {
        std::weak_ptr<Node> parent;
        std::vector<std::shared_ptr<Node>> children;

        glm::mat4 localTransform;
        glm::mat4 worldTransform;
        void refreshTransform(const glm::mat4& parentMatrix);

        void draw(const glm::mat4& topMatrix, DrawContext& ctx) override;
        void draw(const glm::mat4& topMatrix, DrawContext& ctx,
            std::shared_ptr<Material>& materialOverride) override;
    };
}
