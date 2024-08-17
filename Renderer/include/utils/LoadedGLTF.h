#pragma once
#include <unordered_map>

#include "nodes/Node.h"
#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"
#include <core/Buffer.h>
#include <glm/fwd.hpp>

#include "descriptor/DescriptorAllocatorGrowable.h"
#include "core/Image.h"
#include "nodes/IRenderable.h"


namespace Imp::Render
{

    struct LoadedGLTF : public IRenderable
    {
        //DescriptorAllocatorGrowable descriptorPool;
        // storage for all the data on a given glTF file
        std::unordered_map<std::string, std::shared_ptr<Mesh>> meshes;
        std::unordered_map<std::string, std::shared_ptr<Node>> nodes;
        std::unordered_map<std::string, SharedImage> images;
        std::unordered_map<std::string, std::shared_ptr<Material>> materials;
        std::vector <std::string> nodeNames;
        // nodes that dont have a parent, for iterating through the file in tree order
        std::vector<std::shared_ptr<Node>> topNodes;

        std::vector<vk::UniqueSampler> samplers;

        std::shared_ptr<Node> getNode (const std::string& name) const;
        Node& getNodeRef(const std::string& name) const;

        SharedBuffer materialDataBuffer;

        class VKRenderer* creator;

        ~LoadedGLTF() override { };

        virtual void draw(const glm::mat4& topMatrix, DrawContext& ctx) override;
         void draw(const glm::mat4& topMatrix, DrawContext& ctx,SharedMaterial& materialOverride) override;

    private:

        void clearAll();
    };
    using SharedGLTF = std::shared_ptr<LoadedGLTF>;
}
