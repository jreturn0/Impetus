
#pragma once
#include <glm/vec4.hpp>
#include <memory>
#include <array>
#include <string>
#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"
#include "core/ComputeEffect.h"

#include "utils/Forwards.h"
namespace Imp::Render {

    using UniqueComputePipeline = std::unique_ptr<class ComputePipeline>;
    
    class ComputePipeline : public ComputeEffect 
    {
    private:
        vk::UniqueDescriptorSet descriptorSet;
        vk::UniqueDescriptorSetLayout descriptorSetLayout;
        vk::UniquePipelineLayout pipelineLayout;
        vk::UniquePipeline pipeline;

        ComputePipeline(const Device& device, const Image& image, DescriptorAllocatorGrowable& descriptorAllocator,
                        const std::string& shaderName);

        friend UniqueComputePipeline CreateUniqueComputePipeline(const Device& device, const Image& image, DescriptorAllocatorGrowable& descriptorAllocator, const std::string& shaderName);

    public:
        ComputePipeline()=default;
        ~ComputePipeline() = default;
        vk::DescriptorSet& getDescriptorSet() { return *descriptorSet; };
        vk::DescriptorSetLayout& getDescriptorSetLayout() { return *descriptorSetLayout; };
        vk::PipelineLayout& getPipelineLayout() { return *pipelineLayout; };
        vk::Pipeline& getPipeline() { return *pipeline; };
        void recreate(const Device& device, const Image& image, DescriptorAllocatorGrowable& descriptorAllocator);

        void bind(const vk::CommandBuffer& cmd);
        void dispatch(const vk::CommandBuffer& cmd, uint32_t x, uint32_t y, uint32_t z);
    };

    UniqueComputePipeline CreateUniqueComputePipeline(const Device& device, const Image& image, DescriptorAllocatorGrowable& descriptorAllocator, const std::string& shaderName);
}
