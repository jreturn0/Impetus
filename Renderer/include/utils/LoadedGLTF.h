#pragma once
#include <unordered_map>

#include "nodes/Node.h"
#include <core/Buffer.h>

#include "Forwards.h"
#include "nodes/IRenderable.h"
#include "StringUtils.h"
#include <functional>
#include <glm/ext/matrix_float4x4.hpp>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include <vulkan/vulkan_handles.hpp>

namespace imp::gfx
{

    struct LoadedGLTF : public IRenderable
    {
        template <typename T>
        using UnorderedMap = std::unordered_map<std::string, T, utl::TransparentStringHash, std::equal_to<>>;
        //DescriptorAllocatorGrowable descriptorPool;
        // storage for all the data on a given glTF file
        UnorderedMap<std::shared_ptr<Mesh>> meshes;
        UnorderedMap<std::shared_ptr< Node>> nodes;
        UnorderedMap<SharedImage> images;
        UnorderedMap<SharedMaterial> materials;
        std::vector <std::string> nodeNames;
        // nodes that dont have a parent, for iterating through the file in tree order
        std::vector<std::shared_ptr<Node>> topNodes;

        std::vector<vk::raii::Sampler> samplers;

        std::shared_ptr<Node> getNode(std::string_view name) const;
        Node& getNodeRef(std::string_view name) const;

        std::shared_ptr<Buffer> materialDataBuffer;

        //class VKRenderer* creator;

        ~LoadedGLTF() override {};

        virtual void draw(const glm::mat4& topMatrix, DrawContext& ctx) override;
        void draw(const glm::mat4& topMatrix, DrawContext& ctx, SharedMaterial& materialOverride) override;

    private:

        void clearAll();
    };
    using SharedGLTF = std::shared_ptr<LoadedGLTF>;
}
