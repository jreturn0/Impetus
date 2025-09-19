#include "core/ComputePipeline.h"
#include "utils/descriptor/DescriptorLayoutBuilder.h"
#include "core/VulkanContext.h"
#include "utils/descriptor/DescriptorAllocator.h"
#include "core/Image.h"
#include "utils/descriptor/DescriptorAllocatorGrowable.h"
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


    vk::WriteDescriptorSet descriptorWrite{};
    descriptorWrite.setDstSet(m_descriptorSet)
        .setDescriptorType(vk::DescriptorType::eStorageImage)
        .setImageInfo(imageInfo);

    device.updateDescriptorSets({ descriptorWrite }, nullptr);
    vk::PushConstantRange pushRange{ vk::ShaderStageFlagBits::eCompute, 0, sizeof(PushConstants) };

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.setSetLayouts(*m_descriptorSetLayout)
        .setPushConstantRanges(pushRange);

    m_pipelineLayout = device.createPipelineLayout({ {}, *m_descriptorSetLayout, pushRange }, nullptr);


    auto computeShader = ShaderLoader::CreateShaderModule(device, getName(), vkutil::ShaderStage::Compute);


    vk::PipelineShaderStageCreateInfo shaderStageInfo{};
    shaderStageInfo.setStage(vk::ShaderStageFlagBits::eCompute)
        .setModule(computeShader)
        .setPName("main");

    vk::ComputePipelineCreateInfo pipelineInfo{};
    pipelineInfo.setStage(shaderStageInfo)
        .setLayout(m_pipelineLayout);
    try {
        m_pipeline = device.createComputePipeline(nullptr, pipelineInfo);
    }
    catch (vk::SystemError& err) {
        Debug::FatalError("Failed to create compute pipeline: {}", err.what());
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




