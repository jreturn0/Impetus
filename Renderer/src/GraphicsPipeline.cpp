//#include "core/GraphicsPipeline.h"
//
//#include <bitset>
//#include "fstream"
//#include "FSNavigator.h"
//#include "gpudata/GPUDrawPushConstants.h"
//#include "core/SwapChain.h"
//
//#include "geometry/Vertex.h"
//#include "utils/shader/ShaderReflect.h"
//#include "utils/shader/ShaderLoader.h"
//#include "utils/PipelineBuilder.h"
//
//imp::gfx::GraphicsPipeline::GraphicsPipeline(const vk::raii::Device& device,
//    std::vector<vk::DescriptorSetLayout> descriptorSetLayouts,
//    vk::PushConstantRange pushRange,
//    const std::string& shader,
//    vk::PrimitiveTopology topology,
//    vk::PolygonMode polygonMode,
//    vk::CullModeFlags cullMode,
//    vk::FrontFace frontFace,
//    vk::Format colorFormat,
//    vk::Format depthFormat,
//    bool enableDepthTest,
//    bool enableBlending,
//    bool enableMultisampling)
//{
//    // Create the shader modules
//    auto vertModule = ShaderLoader::CreateShaderModule(device, shader, vkutil::ShaderStage::Vertex);
//    auto fragModule = ShaderLoader::CreateShaderModule(device, shader, vkutil::ShaderStage::Fragment);
//    // Validate descriptor set layouts
//    for (auto& layout : descriptorSetLayouts) {
//        if (!layout) {
//            Debug::Throw("Invalid descriptor set layout");
//        }
//    }
//
//    // Create the pipeline layout
//    vk::PipelineLayoutCreateInfo layoutInfo({}, descriptorSetLayouts, pushRange);
//    m_layout = device.createPipelineLayout(layoutInfo);
//    m_pipeline = m_builder.setShaderStages(*vertModule, *fragModule)
//        .setInputTopology(topology)
//        .setPolygonMode(polygonMode)
//        .setCullMode(cullMode, frontFace)
//        .setMultisampling(enableMultisampling)
//        .setDepthTest(enableDepthTest, true, vk::CompareOp::eLessOrEqual)
//        .setBlendingMode(enableBlending ? PipelineBuilder::BlendingMode::AlphaBlend : PipelineBuilder::BlendingMode::None)
//        .setColorAttachmentFormat(colorFormat)
//        .setDepthFormat(depthFormat)
//        .buildPipeline(device, *m_layout);
//}
//
////Imp::Render::GraphicsPipeline::GraphicsPipeline(const vk::raii::Device& device, PipelineBuilder& builder, vk::PipelineLayout layout) : m_layout(layout)
////{
////    m_pipeline = builder.buildPipeline(device, layout);
////}
//
//imp::gfx::GraphicsPipeline::~GraphicsPipeline()
//{
//}
//
//void imp::gfx::GraphicsPipeline::bind( vk::CommandBuffer cmd) const
//{
//    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, *m_pipeline);
//}
//
//
//
