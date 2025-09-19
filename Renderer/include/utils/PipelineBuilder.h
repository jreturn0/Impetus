#pragma once
#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"

namespace imp::gfx
{
    class PipelineBuilder
    {
    public:
        enum class BlendingMode
        {
            None,
            Additive,
            AlphaBlend
        };


        PipelineBuilder();
        PipelineBuilder(const PipelineBuilder&) = delete;
        PipelineBuilder& operator=(const PipelineBuilder&) = delete;
        PipelineBuilder(PipelineBuilder&&) = delete;
        PipelineBuilder& operator=(PipelineBuilder&&) = delete;

        vk::raii::Pipeline buildPipeline(const vk::raii::Device& device, vk::PipelineLayout layout) const;

        void clear();
        PipelineBuilder& setBlendingMode(BlendingMode mode);
        PipelineBuilder& setShaderStages(vk::ShaderModule vertex, vk::ShaderModule fragment);
        PipelineBuilder& setInputTopology(vk::PrimitiveTopology topology);
        PipelineBuilder& setPolygonMode(vk::PolygonMode mode);
        PipelineBuilder& setCullMode(vk::CullModeFlags mode, vk::FrontFace frontFace);
        PipelineBuilder& setColorAttachmentFormat(vk::Format format);
        PipelineBuilder& setMultisampling(vk::SampleCountFlagBits samples);
        PipelineBuilder& setDepthFormat(vk::Format format);
        PipelineBuilder& setDepthTest(bool depthTestEnable, bool depthWriteEnable, vk::CompareOp op);


    private:

        std::vector<vk::PipelineShaderStageCreateInfo> m_shaderStages;
        vk::PipelineInputAssemblyStateCreateInfo m_inputAssembly;
        vk::PipelineRasterizationStateCreateInfo m_rasterizer;
        vk::PipelineColorBlendAttachmentState m_colorBlendAttachment;
        vk::PipelineMultisampleStateCreateInfo m_multisampling;
        vk::PipelineDepthStencilStateCreateInfo m_depthStencil;
        vk::PipelineRenderingCreateInfoKHR m_renderInfo;
        vk::Format m_colorFormat;
    };
}
