#include "pch.h"
#include "core/ComputePipeline.h"
#include "utils/descriptor/DescriptorLayoutBuilder.h"
#include "core/Device.h"
#include "utils/descriptor/DescriptorAllocator.h"
#include "core/Image.h"
#include "utils/descriptor/DescriptorAllocatorGrowable.h"
#include "utils/shader/ShaderLoader.h"

namespace Imp::Render {

	ComputePipeline::ComputePipeline(const Device& device, const Image& image, DescriptorAllocatorGrowable& descriptorAllocator, const std::string& shaderName) :
		ComputeEffect(shaderName)
	{
		{
			DescriptorLayoutBuilder builder;
			builder.addBinding(0, vk::DescriptorType::eStorageImage);
			descriptorSetLayout = builder.build(device.getLogical(), vk::ShaderStageFlagBits::eCompute);
		}

		descriptorSet = descriptorAllocator.allocate(device, descriptorSetLayout.get());
		vk::DescriptorImageInfo imageInfo({}, image.getView(), vk::ImageLayout::eGeneral);

		vk::WriteDescriptorSet descriptorWrite(*descriptorSet, 0, 0, vk::DescriptorType::eStorageImage, imageInfo);
		device.getLogical().updateDescriptorSets({ descriptorWrite }, nullptr);

		vk::PushConstantRange pushess(vk::ShaderStageFlagBits::eCompute, 0, sizeof(PushConstants));

		vk::PipelineLayoutCreateInfo pipelineLayoutInfo({}, *descriptorSetLayout, pushess);

		pipelineLayout = device.getLogical().createPipelineLayoutUnique(pipelineLayoutInfo, nullptr);

		auto computeShader = ShaderLoader::CreateShaderModule(device, shaderName, vkutil::ShaderStage::Compute);

		vk::PipelineShaderStageCreateInfo shaderStageInfo({}, vk::ShaderStageFlagBits::eCompute, *computeShader, "main");
		vk::ComputePipelineCreateInfo pipelineInfo({}, shaderStageInfo, *pipelineLayout);

		auto result = device.getLogical().createComputePipelineUnique(nullptr, pipelineInfo);
		vkutil::CheckResult(result.result);
		pipeline = std::move(result.value);
	}


	void ComputePipeline::recreate(const Device& device, const Image& image,
		DescriptorAllocatorGrowable& descriptorAllocator)
	{
		
		{
			DescriptorLayoutBuilder builder;
			builder.addBinding(0, vk::DescriptorType::eStorageImage);
			descriptorSetLayout = builder.build(device.getLogical(), vk::ShaderStageFlagBits::eCompute);
		}

		descriptorSet = descriptorAllocator.allocate(device, descriptorSetLayout.get());
		vk::DescriptorImageInfo imageInfo({}, image.getView(), vk::ImageLayout::eGeneral);

		vk::WriteDescriptorSet descriptorWrite(*descriptorSet, 0, 0, vk::DescriptorType::eStorageImage, imageInfo);
		device.getLogical().updateDescriptorSets({ descriptorWrite }, nullptr);

		vk::PushConstantRange pushess(vk::ShaderStageFlagBits::eCompute, 0, sizeof(PushConstants));

		vk::PipelineLayoutCreateInfo pipelineLayoutInfo({}, *descriptorSetLayout, pushess);

		pipelineLayout = device.getLogical().createPipelineLayoutUnique(pipelineLayoutInfo, nullptr);

		auto computeShader = ShaderLoader::CreateShaderModule(device, getName(), vkutil::ShaderStage::Compute);

		vk::PipelineShaderStageCreateInfo shaderStageInfo({}, vk::ShaderStageFlagBits::eCompute, *computeShader, "main");
		vk::ComputePipelineCreateInfo pipelineInfo({}, shaderStageInfo, *pipelineLayout);

		auto result = device.getLogical().createComputePipelineUnique(nullptr, pipelineInfo);
		vkutil::CheckResult(result.result);
		pipeline = std::move(result.value);
	}

	void ComputePipeline::bind(const vk::CommandBuffer& cmd)
	{
		cmd.bindPipeline(vk::PipelineBindPoint::eCompute, *pipeline);
		cmd.bindDescriptorSets(vk::PipelineBindPoint::eCompute, *pipelineLayout, 0, *descriptorSet, nullptr);
		cmd.pushConstants<PushConstants>(*pipelineLayout, vk::ShaderStageFlagBits::eCompute, 0, pushes);
	}

	void ComputePipeline::dispatch(const vk::CommandBuffer& cmd, uint32_t x, uint32_t y, uint32_t z)
	{
		cmd.dispatch(x, y, z);
	}

	UniqueComputePipeline CreateUniqueComputePipeline(const Device& device, const Image& image, DescriptorAllocatorGrowable& descriptorAllocator, const std::string& shaderName)
	{
		return std::unique_ptr<ComputePipeline>(new ComputePipeline(device, image, descriptorAllocator, shaderName));
	}

} // namespace Imp::Render
