#pragma once

#include <glm/mat4x4.hpp>
#include <memory>
namespace Imp::Render {
    struct DrawContext;
    class IRenderable
    {
    public:
        virtual ~IRenderable() = default;

    private:
        virtual void draw(const glm::mat4& topMatrix, DrawContext& ctx) = 0;
        virtual void draw(const glm::mat4& topMatrix, DrawContext& ctx, std::shared_ptr<struct Material>& materialOverride) = 0;
    };
}