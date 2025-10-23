
#pragma once
#include "core/ComputeEffect.h"
#include "utils/Forwards.h"
#include "utils/QuickMacros.h"
#include "utils/VKCommon.hpp"
#include <array>
#include <glm/vec4.hpp>
#include <memory>
#include <string>

namespace imp::gfx {

    using UniqueComputePipeline = std::unique_ptr<class ComputePipeline>;

    // Simple implementation of ComputeEffect in Vulkan using a compute pipeline
    class ComputePipeline : public ComputeEffect
    {
    public:
        ComputePipeline(const vk::raii::Device& device, const Image& image, DescriptorAllocatorGrowable& descriptorAllocator, std::string_view shaderName);
        ComputePipeline() = default;
        ~ComputePipeline() = default;

        // Getters

        inline vk::DescriptorSet getDescriptorSet() const noexcept { return m_descriptorSet; };
        inline vk::DescriptorSetLayout getDescriptorSetLayout()const noexcept { return m_descriptorSetLayout; };
        inline vk::PipelineLayout getPipelineLayout()const noexcept { return m_pipelineLayout; };
        inline vk::Pipeline getPipeline() const noexcept { return m_pipeline; };


        // Methods

        void recreate(const vk::raii::Device& device, const Image& image, DescriptorAllocatorGrowable& descriptorAllocator);
        void bind(vk::CommandBuffer cmd);
        void dispatch(vk::CommandBuffer cmd, uint32_t x, uint32_t y, uint32_t z);
    private:
        vk::raii::DescriptorSet m_descriptorSet{ nullptr };
        vk::raii::DescriptorSetLayout m_descriptorSetLayout{ nullptr };
        vk::raii::PipelineLayout m_pipelineLayout{ nullptr };
        vk::raii::Pipeline m_pipeline{ nullptr };
    };


}
