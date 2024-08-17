//#pragma once
//#include <glm/mat4x4.hpp>
//
//#include "geometry/GeoSurface.h"
//#include "utils/VKCommon.hpp"
//#include "utils/QuickMacros.h"
//
//namespace Imp::Render
//{
//	struct Material;
//	struct Mesh;
//	struct DrawContext;
//
//    // base class for a renderable dynamic object
//    class IRenderable
//    {
//    public:
//	    virtual ~IRenderable() = default;
//
//    private:
//	    virtual void Draw(const glm::mat4& topMatrix, DrawContext& ctx) = 0;
//    };
//
//    // implementation of a drawable scene node.
//    // the scene node can hold children and will also keep a transform to propagate
//    // to them
//    struct Node : public IRenderable
//    {
//
//        // parent pointer must be a weak pointer to avoid circular dependencies
//        std::weak_ptr<Node> parent;
//        std::vector<std::shared_ptr<Node>> children;
//
//        glm::mat4 localTransform;
//        glm::mat4 worldTransform;
//
//        void refreshTransform(const glm::mat4& parentMatrix)
//        {
//            worldTransform = parentMatrix * localTransform;
//            for (auto c : children) {
//                c->refreshTransform(worldTransform);
//            }
//        }
//
//        void draw(const glm::mat4& topMatrix, DrawContext& ctx) override
//        {
//            // draw children
//            for (const auto& c : children) {
//                c->Draw(topMatrix, ctx);
//            }
//        }
//    };
//    struct MeshNode : public Node
//    {
//
//        std::shared_ptr<Mesh> mesh;
//
//        void Draw(const glm::mat4& topMatrix, DrawContext& ctx) override;
//    };
//
//    struct RenderObject
//    {
//        uint32_t indexCount;
//        uint32_t firstIndex;
//        vk::Buffer indexBuffer;
//
//        std::shared_ptr<Material> material;
//        Bounds bounds;
//        glm::mat4 transform;
//        vk::DeviceAddress vertexBufferAddress;
//    };
//
//    struct DrawContext
//    {
//        std::vector<RenderObject> OpaqueSurfaces;
//        std::vector<RenderObject> TransparentSurfaces;
//    };
//
//}
