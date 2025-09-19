#include "materials/MetallicRoughness.h"

#include "VKRenderer.h"
#include "utils/descriptor//DescriptorLayoutBuilder.h"
#include "gpudata/GPUDrawPushConstants.h"
#include "utils/PipelineBuilder.h"
#include "utils/descriptor/DescriptorAllocatorGrowable.h"
#include "core/Buffer.h"
#include "core/Image.h"
#include "materials/Material.h"
#include "utils/shader/ShaderLoader.h"

imp::gfx::MetallicRoughness::MetallicRoughness(const vk::raii::Device& device, vk::DescriptorSetLayout sceneDataLayout, vk::Format drawFormat, vk::Format depthFormat)
{

    DescriptorLayoutBuilder layoutBuilder;
    layoutBuilder.addBinding(0, vk::DescriptorType::eUniformBuffer);
    layoutBuilder.addBinding(1, vk::DescriptorType::eCombinedImageSampler);
    layoutBuilder.addBinding(2, vk::DescriptorType::eCombinedImageSampler);

    pipelineBuilder.setInputTopology(vk::PrimitiveTopology::eTriangleList)
        .setPolygonMode(vk::PolygonMode::eFill)
        .setMultisampling(vk::SampleCountFlagBits::e1);

    materialLayout = layoutBuilder.build(device, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);

    buildPipelines(device, sceneDataLayout, drawFormat, depthFormat);
}

void imp::gfx::MetallicRoughness::recreate(const vk::raii::Device& device, vk::DescriptorSetLayout sceneDataLayout, vk::Format drawFormat, vk::Format depthFormat)
{
    buildPipelines(device, sceneDataLayout, drawFormat, depthFormat);
}

void imp::gfx::MetallicRoughness::buildPipelines(const vk::raii::Device& device, vk::DescriptorSetLayout sceneDataLayout, vk::Format drawFormat, vk::Format depthFormat)
{
    constexpr vk::PushConstantRange matrixRange{ vk::ShaderStageFlagBits::eVertex,0 ,sizeof(GPUDrawPushConstants) };
    auto vertModule = ShaderLoader::CreateShaderModule(device, "mesh_material", vkutil::ShaderStage::Vertex);
    auto fragModule = ShaderLoader::CreateShaderModule(device, "mesh_material", vkutil::ShaderStage::Fragment);
    std::vector layouts = {
        sceneDataLayout,*materialLayout
    };
    for (auto& layout : layouts) {
        if (!layout) {
            Debug::Throw("Invalid descriptor set layout");
        }
    }
    auto pipelineCreateInfo = vk::PipelineLayoutCreateInfo{}
        .setSetLayouts(layouts)
        .setPushConstantRanges(matrixRange);

    pipelineLayout = device.createPipelineLayout(pipelineCreateInfo);

    opaquePipeline = pipelineBuilder.setShaderStages(*vertModule, *fragModule)
        .setCullMode(vk::CullModeFlagBits::eBack, vk::FrontFace::eCounterClockwise)
        .setDepthTest(true, true, vk::CompareOp::eLessOrEqual)
        .setBlendingMode(PipelineBuilder::BlendingMode::None)
        .setColorAttachmentFormat(drawFormat)
        .setDepthFormat(depthFormat)
        .buildPipeline(device, *pipelineLayout);

    transparentPipeline= pipelineBuilder.setDepthTest(true,false,vk::CompareOp::eLess)
        .setCullMode(vk::CullModeFlagBits::eNone, vk::FrontFace::eClockwise)
        .setBlendingMode(PipelineBuilder::BlendingMode::Additive)
        .buildPipeline(device, *pipelineLayout);
}



std::shared_ptr<imp::gfx::Material> imp::gfx::MetallicRoughness::writeMaterial(const vk::raii::Device& device,
    MaterialPass pass, MaterialResources& resources, DescriptorAllocatorGrowable& descriptorAllocator)
{
    auto material = std::make_shared<Material>();
    material->passType = pass;
    material->pipeline = pass == MaterialPass::Transparent ? transparentPipeline : opaquePipeline;
    material->pipelineLayout = pipelineLayout;
    material->set = descriptorAllocator.allocate(device, *materialLayout);

    writer.clear();
    writer.writeBuffer(0, resources.dataBuffer->getBuffer(), sizeof(MaterialConstants), resources.dataOffset, vk::DescriptorType::eUniformBuffer);
    writer.writeImage(1, resources.colorImage.lock()->getView(), resources.colorSampler, vk::ImageLayout::eShaderReadOnlyOptimal, vk::DescriptorType::eCombinedImageSampler);
    writer.writeImage(2, resources.metalRoughImage.lock()->getView(), resources.metalRoughSampler, vk::ImageLayout::eShaderReadOnlyOptimal, vk::DescriptorType::eCombinedImageSampler);
    writer.updateSet(device, *material->set);


    material->buffer = resources.dataBuffer;
    return material;
}



