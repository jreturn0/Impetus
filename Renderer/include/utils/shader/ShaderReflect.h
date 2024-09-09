#pragma once
#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"

namespace Imp::Render
{
	class ShaderReflect
	{
	private:


	public:
		DISABLE_COPY_AND_MOVE(ShaderReflect);
		
	};

	struct ReflectInfo
	{
		std::vector<vk::PushConstantRange> pushConstantRanges;
		std::vector<vk::UniqueDescriptorSetLayout> descriptorSetLayouts;
	};

	std::vector<uint32_t> readSPVFile(const std::string& filename);

	void ReflectShader(const std::vector<uint32_t>& spirvCode);

	ReflectInfo ReflectShader(vk::Device device, const std::vector<uint32_t>& spirv);

	std::vector<vk::UniqueDescriptorSetLayout> CreateDescriptorSetLayoutsFromShader(const vk::Device& device, const std::vector<uint32_t>& spirvCode, vk::ShaderStageFlagBits shaderStage);

	std::vector<vk::PushConstantRange> CreatePushConstantRangesFromShader(const std::vector<uint32_t>& spirvCode, vk::ShaderStageFlagBits shaderStage);

	vk::UniquePipelineLayout CreatePipelineLayoutFromShaders(
	    const vk::Device& device,
	    const std::vector<std::vector<uint8_t>>& spirvBinaries, const std::vector<vk::ShaderStageFlagBits>& shaderStages);


    vk::UniquePipelineLayout CreatePipelineLayout(const vk::Device& device, const std::vector<vk::DescriptorSetLayout>& descriptorSetLayouts);
}
