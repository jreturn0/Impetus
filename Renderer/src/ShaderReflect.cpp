#include "pch.h"
#include "utils/shader/ShaderReflect.h"
#include <vulkan/vulkan.hpp>
#include <spirv_cross/spirv.hpp>
#include <fstream>
#include <vector>
#include <iostream>
#include <spirv-reflect/spirv_reflect.h>

// Function to read SPIR-V binary from file
std::vector<uint32_t> Imp::Render::readSPVFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open SPIR-V file.");
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));

    file.seekg(0);
    file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
    file.close();

    return buffer;
}
void Imp::Render::ReflectShader(const std::vector<uint32_t>& spirvCode)
{

    SpvReflectShaderModule module;
    SpvReflectResult result = spvReflectCreateShaderModule(
        spirvCode.size() * sizeof(uint32_t),
        spirvCode.data(),
        &module
    );
    if (result != SPV_REFLECT_RESULT_SUCCESS) {
        throw std::runtime_error("Failed to create SPIR-V reflect module.");
    }

    uint32_t descriptorSetCount = 0;
    result = spvReflectEnumerateDescriptorSets(&module, &descriptorSetCount, nullptr);
    if (result != SPV_REFLECT_RESULT_SUCCESS) {
        throw std::runtime_error("Failed to enumerate descriptor sets.");
    }

    std::vector<SpvReflectDescriptorSet*> descriptorSets(descriptorSetCount);
    result = spvReflectEnumerateDescriptorSets(&module, &descriptorSetCount, descriptorSets.data());
    if (result != SPV_REFLECT_RESULT_SUCCESS) {
        throw std::runtime_error("Failed to enumerate descriptor sets.");
    }

    for (uint32_t i = 0; i < descriptorSetCount; ++i) {
        const SpvReflectDescriptorSet& set = *(descriptorSets[i]);
        std::cout << "Descriptor Set " << set.set << "\n";
        for (uint32_t j = 0; j < set.binding_count; ++j) {
            const SpvReflectDescriptorBinding& binding = *(set.bindings[j]);
            //std::cout << " Binding " << binding.binding
            //    << " Type " << binding.descriptor_type << "\n";
        }
    }

    spvReflectDestroyShaderModule(&module);




}





std::vector<vk::UniqueDescriptorSetLayout> Imp::Render::CreateDescriptorSetLayoutsFromShader(
	const vk::Device& device,
	const std::vector<uint32_t>& spirvCode,
	vk::ShaderStageFlagBits shaderStage)
{
	SpvReflectShaderModule module;
	SpvReflectResult result = spvReflectCreateShaderModule(
		spirvCode.size() * sizeof(uint32_t),
		spirvCode.data(),
		&module
	);
	if (result != SPV_REFLECT_RESULT_SUCCESS) {
		throw std::runtime_error("Failed to create SPIR-V reflect module.");
	}

	uint32_t descriptorSetCount = 0;
	result = spvReflectEnumerateDescriptorSets(&module, &descriptorSetCount, nullptr);
	if (result != SPV_REFLECT_RESULT_SUCCESS) {
		throw std::runtime_error("Failed to enumerate descriptor sets.");
	}

	std::vector<SpvReflectDescriptorSet*> descriptorSets(descriptorSetCount);
	result = spvReflectEnumerateDescriptorSets(&module, &descriptorSetCount, descriptorSets.data());
	if (result != SPV_REFLECT_RESULT_SUCCESS) {
		throw std::runtime_error("Failed to enumerate descriptor sets.");
	}

	std::vector<vk::UniqueDescriptorSetLayout> layouts;
	for (uint32_t i = 0; i < descriptorSetCount; ++i) {
		const SpvReflectDescriptorSet& set = *(descriptorSets[i]);
		std::vector<vk::DescriptorSetLayoutBinding> bindings;
		for (uint32_t j = 0; j < set.binding_count; ++j) {
			const SpvReflectDescriptorBinding& binding = *(set.bindings[j]);
			vk::DescriptorSetLayoutBinding layoutBinding = {};
			layoutBinding.binding = binding.binding;
			layoutBinding.descriptorType = static_cast<vk::DescriptorType>(binding.descriptor_type);
			layoutBinding.descriptorCount = binding.count;
			layoutBinding.stageFlags = shaderStage;
			bindings.push_back(layoutBinding);
		}

		vk::DescriptorSetLayoutCreateInfo layoutInfo({}, bindings);
		layouts.push_back(device.createDescriptorSetLayoutUnique(layoutInfo));
	}

	spvReflectDestroyShaderModule(&module);
	return layouts;
}

std::vector<vk::PushConstantRange> Imp::Render::CreatePushConstantRangesFromShader(
	const std::vector<uint32_t>& spirvCode,
	vk::ShaderStageFlagBits shaderStage)
{
	SpvReflectShaderModule module;
	SpvReflectResult result = spvReflectCreateShaderModule(
		spirvCode.size() * sizeof(uint32_t),
		spirvCode.data(),
		&module
	);
	if (result != SPV_REFLECT_RESULT_SUCCESS) {
		throw std::runtime_error("Failed to create SPIR-V reflect module.");
	}

	uint32_t pushConstantCount = 0;
	result = spvReflectEnumeratePushConstantBlocks(&module, &pushConstantCount, nullptr);
	if (result != SPV_REFLECT_RESULT_SUCCESS) {
		throw std::runtime_error("Failed to enumerate push constant blocks.");
	}

	std::vector<SpvReflectBlockVariable*> pushConstants(pushConstantCount);
	result = spvReflectEnumeratePushConstantBlocks(&module, &pushConstantCount, pushConstants.data());
	if (result != SPV_REFLECT_RESULT_SUCCESS) {
		throw std::runtime_error("Failed to enumerate push constant blocks.");
	}

	std::vector<vk::PushConstantRange> pushConstantRanges;
	for (uint32_t i = 0; i < pushConstantCount; ++i) {
		const SpvReflectBlockVariable& pc = *(pushConstants[i]);
		vk::PushConstantRange pushConstantRange = {};
		pushConstantRange.stageFlags = shaderStage;
		pushConstantRange.offset = pc.offset;
		pushConstantRange.size = pc.size;
		pushConstantRanges.push_back(pushConstantRange);
	}

	spvReflectDestroyShaderModule(&module);
	return pushConstantRanges;
}


vk::UniquePipelineLayout Imp::Render::CreatePipelineLayoutFromShaders(
    const vk::Device& device,
    const std::vector<std::vector<uint8_t>>& spirvBinaries,
    const std::vector<vk::ShaderStageFlagBits>& shaderStages)
{
    std::vector<vk::DescriptorSetLayout> descriptorSetLayouts;
    std::vector<vk::PushConstantRange> pushConstantRanges;

    for (size_t i = 0; i < spirvBinaries.size(); ++i) {
        const auto& spirvCode = spirvBinaries[i];
        const auto& shaderStage = shaderStages[i];

        SpvReflectShaderModule module;
        SpvReflectResult result = spvReflectCreateShaderModule(spirvCode.size(), spirvCode.data(), &module);
        if (result != SPV_REFLECT_RESULT_SUCCESS) {
            std::cerr << "Failed to create shader module!" << std::endl;
            throw std::runtime_error("Failed to create shader module!");
        }

        // Retrieve descriptor sets
        uint32_t setCount = 0;
        result = spvReflectEnumerateDescriptorSets(&module, &setCount, nullptr);
        if (result != SPV_REFLECT_RESULT_SUCCESS) {
            spvReflectDestroyShaderModule(&module);
            std::cerr << "Failed to enumerate descriptor sets!" << std::endl;
            throw std::runtime_error("Failed to enumerate descriptor sets!");
        }

        std::vector<SpvReflectDescriptorSet*> sets(setCount);
        result = spvReflectEnumerateDescriptorSets(&module, &setCount, sets.data());
        if (result != SPV_REFLECT_RESULT_SUCCESS) {
            spvReflectDestroyShaderModule(&module);
            std::cerr << "Failed to enumerate descriptor sets!" << std::endl;
            throw std::runtime_error("Failed to enumerate descriptor sets!");
        }

        for (const auto& set : sets) {
            std::vector<vk::DescriptorSetLayoutBinding> bindings;
            for (uint32_t k = 0; k < set->binding_count; ++k) {
                const SpvReflectDescriptorBinding* reflBinding = set->bindings[k];
                vk::DescriptorSetLayoutBinding layoutBinding = {};
                layoutBinding.binding = reflBinding->binding;
                layoutBinding.descriptorType = static_cast<vk::DescriptorType>(reflBinding->descriptor_type);
                layoutBinding.descriptorCount = reflBinding->count;
                layoutBinding.stageFlags = shaderStage;
                bindings.push_back(layoutBinding);
            }

            vk::DescriptorSetLayoutCreateInfo layoutInfo({}, bindings);
            descriptorSetLayouts.push_back(device.createDescriptorSetLayout(layoutInfo));
        }

        // Retrieve push constants
        uint32_t pushConstantCount = 0;
        result = spvReflectEnumeratePushConstantBlocks(&module, &pushConstantCount, nullptr);
        if (result != SPV_REFLECT_RESULT_SUCCESS) {
            spvReflectDestroyShaderModule(&module);
            std::cerr << "Failed to enumerate push constant blocks!" << std::endl;
            throw std::runtime_error("Failed to enumerate push constant blocks!");
        }

        std::vector<SpvReflectBlockVariable*> pushConstants(pushConstantCount);
        result = spvReflectEnumeratePushConstantBlocks(&module, &pushConstantCount, pushConstants.data());
        if (result != SPV_REFLECT_RESULT_SUCCESS) {
            spvReflectDestroyShaderModule(&module);
            std::cerr << "Failed to enumerate push constant blocks!" << std::endl;
            throw std::runtime_error("Failed to enumerate push constant blocks!");
        }

        for (const auto& pc : pushConstants) {
            vk::PushConstantRange pushConstantRange = {};
            pushConstantRange.stageFlags = shaderStage;
            pushConstantRange.offset = pc->offset;
            pushConstantRange.size = pc->size;
            pushConstantRanges.push_back(pushConstantRange);
        }

        spvReflectDestroyShaderModule(&module);
    }

    vk::PipelineLayoutCreateInfo layoutInfo({}, descriptorSetLayouts, pushConstantRanges);
    return device.createPipelineLayoutUnique(layoutInfo);
}