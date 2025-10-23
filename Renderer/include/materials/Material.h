#pragma once
#include <glm/fwd.hpp>
#include "utils/VKCommon.hpp"
#include "MaterialPass.h"

namespace imp::gfx {
    class Buffer;
    // Structure representing a shader surface material 
    struct Material
    {

        std::string name{};
        vk::Pipeline pipeline{ nullptr };
        vk::PipelineLayout pipelineLayout{ nullptr };
        vk::raii::DescriptorSet set{ nullptr };
        MaterialPass passType{};
        std::shared_ptr<Buffer> buffer{ nullptr };


        Material(vk::Pipeline pipeline, vk::raii::DescriptorSet& set, MaterialPass passType, std::shared_ptr<Buffer> buffer) :
            pipeline(pipeline),
            set(std::move(set)),
            passType(passType),
            buffer(std::move(buffer))
        {
        }
        Material()
        {
            pipeline = nullptr;
            passType = MaterialPass::Opaque;
        }
        Material(const Material&) = delete;
        Material& operator=(const Material&) = delete;
        Material(Material&&) = delete;
        Material& operator=(Material&&) = delete;
    };
    using SharedMaterial = std::shared_ptr<Material>;




}
