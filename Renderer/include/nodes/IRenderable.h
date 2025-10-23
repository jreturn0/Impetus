#pragma once
#include <glm/mat4x4.hpp>
#include <memory>

namespace imp::gfx {
    struct DrawContext;
    struct Material;
    // Interface for renderable objects
    class IRenderable
    {
    public:
        virtual ~IRenderable() = default;
    private:

        // Virtual Methods

        virtual void draw(const glm::mat4& topMatrix, DrawContext& ctx) = 0;
        virtual void draw(const glm::mat4& topMatrix, DrawContext& ctx, std::shared_ptr<Material>& materialOverride) = 0;
    };
}