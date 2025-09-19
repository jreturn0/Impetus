#include "utils/PipelineBuilder.h"


imp::gfx::PipelineBuilder::PipelineBuilder() : m_shaderStages({}), m_inputAssembly({}), m_rasterizer({}), m_colorBlendAttachment({}), m_multisampling({}), m_depthStencil({}), m_renderInfo({}), m_colorFormat(vk::Format::eUndefined)
{

}

void imp::gfx::PipelineBuilder::clear()
{
    m_shaderStages.clear();
    m_inputAssembly = vk::PipelineInputAssemblyStateCreateInfo{};
    m_rasterizer = vk::PipelineRasterizationStateCreateInfo{};
    m_colorBlendAttachment = vk::PipelineColorBlendAttachmentState{};
    m_multisampling = vk::PipelineMultisampleStateCreateInfo{};
    m_depthStencil = vk::PipelineDepthStencilStateCreateInfo{};
    m_renderInfo = vk::PipelineRenderingCreateInfo{};
    m_colorFormat = vk::Format::eUndefined;

}

vk::raii::Pipeline imp::gfx::PipelineBuilder::buildPipeline(const vk::raii::Device& device, vk::PipelineLayout layout) const
{
    auto viewportState = vk::PipelineViewportStateCreateInfo{}
        .setViewportCount(1)
        .setPViewports(nullptr)
        .setScissorCount(1)
        .setPScissors(nullptr);
    auto colorBlending = vk::PipelineColorBlendStateCreateInfo{}
        .setLogicOpEnable(vk::False)
        .setLogicOp(vk::LogicOp::eCopy)
        .setAttachments(m_colorBlendAttachment);
    vk::PipelineVertexInputStateCreateInfo vertexInput{};
    vk::DynamicState dynamicStates[] = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
    vk::PipelineDynamicStateCreateInfo dynamicState({}, dynamicStates);
    auto pipelineInfo = vk::GraphicsPipelineCreateInfo{}
        .setStages(m_shaderStages)
        .setPVertexInputState(&vertexInput)
        .setPInputAssemblyState(&m_inputAssembly)
        .setPViewportState(&viewportState)
        .setPRasterizationState(&m_rasterizer)
        .setPMultisampleState(&m_multisampling)
        .setPDepthStencilState(&m_depthStencil)
        .setPColorBlendState(&colorBlending)
        .setPDynamicState(&dynamicState)
        .setLayout(layout)
        .setRenderPass(nullptr)
        .setSubpass(0)
        .setBasePipelineHandle(nullptr)
        .setBasePipelineIndex(-1)
        .setPNext(&m_renderInfo);




    vk::raii::Pipeline pipeline{ nullptr };
    try {
        pipeline = device.createGraphicsPipeline(nullptr, pipelineInfo);
    }
    catch (const vk::SystemError& err) {
        Debug::FatalError("Failed to create graphics pipeline: {}", err.what());
    }

    return pipeline;
}

imp::gfx::PipelineBuilder& imp::gfx::PipelineBuilder::setBlendingMode(BlendingMode mode)
{
    switch (mode) {
    case BlendingMode::None:
        m_colorBlendAttachment.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
            .setBlendEnable(vk::False);
        return *this;
    case BlendingMode::Additive:
        m_colorBlendAttachment.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
            .setBlendEnable(vk::True)
            .setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha)
            .setDstColorBlendFactor(vk::BlendFactor::eOne)
            .setColorBlendOp(vk::BlendOp::eAdd)
            .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
            .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
            .setAlphaBlendOp(vk::BlendOp::eAdd);
        return *this;
    case BlendingMode::AlphaBlend:
        m_colorBlendAttachment.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
            .setBlendEnable(vk::True)
            .setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha)
            .setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
            .setColorBlendOp(vk::BlendOp::eAdd)
            .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
            .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
            .setAlphaBlendOp(vk::BlendOp::eAdd);
        return *this;
    default:
        Debug::FatalError("Unknown blending mode");
        return *this;
    }
}



imp::gfx::PipelineBuilder& imp::gfx::PipelineBuilder::setColorAttachmentFormat(vk::Format format)
{
    m_colorFormat = format;
    m_renderInfo.setColorAttachmentFormats(m_colorFormat);
    return *this;
}

imp::gfx::PipelineBuilder& imp::gfx::PipelineBuilder::setDepthFormat(vk::Format format)
{
    m_renderInfo.setDepthAttachmentFormat(format);
    return *this;
}

imp::gfx::PipelineBuilder& imp::gfx::PipelineBuilder::setDepthTest(bool depthTestEnable, bool depthWriteEnable, vk::CompareOp op)
{
    m_depthStencil = vk::PipelineDepthStencilStateCreateInfo{}
        .setDepthTestEnable(depthTestEnable)
        .setDepthWriteEnable(depthWriteEnable)
        .setDepthCompareOp(op)
        .setDepthBoundsTestEnable(vk::False)
        .setStencilTestEnable(vk::False)
        .setMinDepthBounds(0.f)        
        .setMaxDepthBounds(1.f);
    return *this;
}

imp::gfx::PipelineBuilder& imp::gfx::PipelineBuilder::setShaderStages(vk::ShaderModule vertex, vk::ShaderModule fragment)
{
    auto vertexStage = vk::PipelineShaderStageCreateInfo{}
        .setStage(vk::ShaderStageFlagBits::eVertex)
        .setModule(vertex)
        .setPName("main");
    auto fragmentStage = vk::PipelineShaderStageCreateInfo{}
        .setStage(vk::ShaderStageFlagBits::eFragment)
        .setModule(fragment)
        .setPName("main");
    m_shaderStages.emplace_back(vertexStage);
    m_shaderStages.emplace_back(fragmentStage);
    return *this;
}

imp::gfx::PipelineBuilder& imp::gfx::PipelineBuilder::setInputTopology(vk::PrimitiveTopology topology)
{
    m_inputAssembly.setTopology(topology)
        .setPrimitiveRestartEnable(vk::False); 
    return *this;
}

imp::gfx::PipelineBuilder& imp::gfx::PipelineBuilder::setPolygonMode(vk::PolygonMode mode)
{
    m_rasterizer.setPolygonMode(mode)
        .setLineWidth(1.f);
    return *this;
}

imp::gfx::PipelineBuilder& imp::gfx::PipelineBuilder::setCullMode(vk::CullModeFlags mode, vk::FrontFace frontFace)
{
    m_rasterizer.setCullMode(mode)
        .setFrontFace(frontFace);
    return *this;
}

imp::gfx::PipelineBuilder& imp::gfx::PipelineBuilder::setMultisampling(vk::SampleCountFlagBits samples)
{
    m_multisampling.setSampleShadingEnable(samples != vk::SampleCountFlagBits::e1)
        .setRasterizationSamples(samples)
        .setMinSampleShading(1.f)
        .setPSampleMask(nullptr)
        .setAlphaToCoverageEnable(vk::False)
        .setAlphaToOneEnable(vk::False);
    return *this;
}
