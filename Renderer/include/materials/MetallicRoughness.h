#pragma once
#include <glm/vec4.hpp>

#include "core/Buffer.h"
#include "Material.h"
#include "utils/descriptor/DescriptorWriter.h"
#include "utils/descriptor/DescriptorWriter.h"
#include "utils/Forwards.h"
#include "utils/PipelineBuilder.h"
#include "utils/QuickMacros.h"
#include "utils/VKCommon.hpp"

namespace imp::gfx
{
    // Implements a PBR Metallic-Roughness material system
    struct MetallicRoughness
    {
        struct MaterialConstants
        {
            glm::vec4 colorFactors{};
            glm::vec4 metalRoughFactors{};
            glm::vec4 extra[14]{};
        };

        struct MaterialResources
        {
            std::weak_ptr<Image> colorImage{};
            vk::Sampler colorSampler{ nullptr };
            std::weak_ptr < Image> metalRoughImage{};
            vk::Sampler metalRoughSampler{ nullptr };
            std::shared_ptr<Buffer> dataBuffer{};
            uint32_t dataOffset{};
        };

        vk::raii::PipelineLayout pipelineLayout{ nullptr };
        PipelineBuilder pipelineBuilder{};
        vk::raii::Pipeline opaquePipeline{ nullptr };        
        vk::raii::Pipeline transparentPipeline{ nullptr };
        vk::raii::DescriptorSetLayout materialLayout{ nullptr };
        DescriptorWriter writer{};

        MetallicRoughness(const vk::raii::Device& device, vk::DescriptorSetLayout sceneDataLayout, vk::Format drawFormat, vk::Format depthFormat);
        ~MetallicRoughness() = default;
        MetallicRoughness(const MetallicRoughness&) = delete;
        MetallicRoughness& operator=(const MetallicRoughness&) = delete;
        MetallicRoughness(MetallicRoughness&&) = delete;
        MetallicRoughness& operator=(MetallicRoughness&&) = delete;

        // Methods

        void recreate(const vk::raii::Device& device, vk::DescriptorSetLayout sceneDataLayout, vk::Format drawFormat, vk::Format depthFormat);
        void buildPipelines(const vk::raii::Device& device, vk::DescriptorSetLayout sceneDataLayout, vk::Format drawFormat, vk::Format depthFormat);
        std::shared_ptr<Material> writeMaterial(const vk::raii::Device& device, MaterialPass pass,
            MaterialResources& resources, DescriptorAllocatorGrowable& descriptorAllocator);
        void updateMaterialPipelines(Material& material) const;

    };
}
