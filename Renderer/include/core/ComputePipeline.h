
#pragma once
#include <glm/vec4.hpp>
#include <memory>
#include <array>
#include <string>
#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"
#include "core/ComputeEffect.h"

#include "utils/Forwards.h"
namespace imp::gfx {

    using UniqueComputePipeline = std::unique_ptr<class ComputePipeline>;

    class ComputePipeline : public ComputeEffect
    {
    public:
        ComputePipeline(const vk::raii::Device& device, const Image& image, DescriptorAllocatorGrowable& descriptorAllocator, std::string_view shaderName);
        ComputePipeline() = default;
        ~ComputePipeline() = default;

        // Getters

        vk::DescriptorSet getDescriptorSet() const noexcept { return m_descriptorSet; };
        vk::DescriptorSetLayout getDescriptorSetLayout()const noexcept { return m_descriptorSetLayout; };
        vk::PipelineLayout getPipelineLayout()const noexcept { return m_pipelineLayout; };
        vk::Pipeline getPipeline() const noexcept { return m_pipeline; };


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
