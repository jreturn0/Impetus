#include "core/ComputePipeline.h"
#include "core/Image.h"
#include "core/VulkanContext.h"
#include "utils/descriptor/DescriptorAllocatorGrowable.h"
#include "utils/descriptor/DescriptorLayoutBuilder.h"
#include "utils/shader/ShaderLoader.h"



imp::gfx::ComputePipeline::ComputePipeline(const vk::raii::Device& device, const Image& image, DescriptorAllocatorGrowable& descriptorAllocator, std::string_view shaderName) :
    ComputeEffect(shaderName)
{
    recreate(device, image, descriptorAllocator);
}


void imp::gfx::ComputePipeline::recreate(const vk::raii::Device& device, const Image& image,
    DescriptorAllocatorGrowable& descriptorAllocator)
{

    {
        DescriptorLayoutBuilder builder;
        builder.addBinding(0, vk::DescriptorType::eStorageImage);
        m_descriptorSetLayout = builder.build(device, vk::ShaderStageFlagBits::eCompute);
    }

    m_descriptorSet = descriptorAllocator.allocate(device, m_descriptorSetLayout);
    vk::DescriptorImageInfo imageInfo({}, image.getView(), vk::ImageLayout::eGeneral);
    const auto descriptorWrite = vk::WriteDescriptorSet{}
        .setDstSet(m_descriptorSet)
        .setDescriptorType(vk::DescriptorType::eStorageImage)
        .setImageInfo(imageInfo);

    device.updateDescriptorSets({ descriptorWrite }, nullptr);
    vk::PushConstantRange pushRange{ vk::ShaderStageFlagBits::eCompute, 0, sizeof(PushConstants) };

    const auto pipelineLayoutInfo = vk::PipelineLayoutCreateInfo{}
        .setSetLayouts(*m_descriptorSetLayout)
        .setPushConstantRanges(pushRange);

    m_pipelineLayout = device.createPipelineLayout({ {}, *m_descriptorSetLayout, pushRange }, nullptr);


    auto computeShader = ShaderLoader::CreateShaderModule(device, getName(), vkutil::ShaderStage::Compute);


    const auto shaderStageInfo = vk::PipelineShaderStageCreateInfo{}
        .setStage(vk::ShaderStageFlagBits::eCompute)
        .setModule(computeShader)
        .setPName("main");

    const auto pipelineInfo = vk::ComputePipelineCreateInfo{}
        .setStage(shaderStageInfo)
        .setLayout(m_pipelineLayout);
    try {
        m_pipeline = device.createComputePipeline(nullptr, pipelineInfo);
    }
    catch (vk::SystemError& err) {
        Debug::Error("Failed to create compute pipeline: {}", err.what());
    }
}

void imp::gfx::ComputePipeline::bind(vk::CommandBuffer cmd)
{
    cmd.bindPipeline(vk::PipelineBindPoint::eCompute, m_pipeline);
    cmd.bindDescriptorSets(vk::PipelineBindPoint::eCompute, m_pipelineLayout, 0, *m_descriptorSet, nullptr);
    cmd.pushConstants<PushConstants>(m_pipelineLayout, vk::ShaderStageFlagBits::eCompute, 0, pushes);
}

void imp::gfx::ComputePipeline::dispatch(vk::CommandBuffer cmd, uint32_t x, uint32_t y, uint32_t z)
{
    cmd.dispatch(x, y, z);
}




