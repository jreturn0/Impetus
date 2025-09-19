#pragma once

#include <glm/mat4x4.hpp>
#include <memory>
namespace imp::gfx {
    struct DrawContext;
    struct Material;
    class IRenderable
    {
    public:
        virtual ~IRenderable() = default;

    private:
        virtual void draw(const glm::mat4& topMatrix, DrawContext& ctx) = 0;
        virtual void draw(const glm::mat4& topMatrix, DrawContext& ctx, std::shared_ptr<Material>& materialOverride) = 0;
    };
}