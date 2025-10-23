#pragma once
#include "Forwards.h"
#include "nodes/IRenderable.h"
#include "nodes/Node.h"
#include "StringUtils.h"
#include "utils/VKCommon.hpp"
#include <core/Buffer.h>
#include <functional>
#include <glm/ext/matrix_float4x4.hpp>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>


namespace imp::gfx
{
    // Structure representing a loaded glTF file with its associated data
    struct LoadedGLTF : public IRenderable
    {
        // Using a transparent hash map for string_view key lookups
        template <typename T>
        using UnorderedMap = std::unordered_map<std::string, T, utl::TransparentStringHash, std::equal_to<>>;
        UnorderedMap<std::shared_ptr<Mesh>> meshes{};
        UnorderedMap<std::shared_ptr< Node>> nodes{};
        UnorderedMap<SharedImage> images{};
        UnorderedMap<SharedMaterial> materials{};
        std::vector <std::string> nodeNames{};        
        std::vector<std::shared_ptr<Node>> topNodes{}; // nodes that dont have a parent, for iterating through the file in tree order
        std::vector<vk::raii::Sampler> samplers{};
        std::shared_ptr<Buffer> materialDataBuffer{ nullptr };


        // Getters 

        std::shared_ptr<Node> getNode(std::string_view name) const;
        Node& getNodeRef(std::string_view name) const;
        
        // Overrides

        virtual void draw(const glm::mat4& topMatrix, DrawContext& ctx) override;
        void draw(const glm::mat4& topMatrix, DrawContext& ctx, SharedMaterial& materialOverride) override;
        ~LoadedGLTF() override {};
    };
    using SharedGLTF = std::shared_ptr<LoadedGLTF>;
}
