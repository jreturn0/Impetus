#include "pch.h"
#include "utils/shader/ShaderLoader.h"

#include <filesystem>
#include <fstream>

#include "Debug.h"
#include "core/Device.h"
#include "utils/shader/ShaderReflect.h"
#include "FileDirectoryHelper.h"

std::vector<uint32_t> Imp::Render::ShaderLoader::readFile(const std::string& filePath)
{
	std::ifstream file(filePath, std::ios::ate | std::ios::binary);
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open file: " + filePath);
	}

	const auto fileSize = (file.tellg());
	std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));
	file.seekg(0);
	file.read(reinterpret_cast<char*>(buffer.data()),fileSize);
	file.close();

	return buffer;
}

std::vector<uint8_t> Imp::Render::ShaderLoader::readFile8(const std::string& filePath)
{
	std::ifstream file(filePath, std::ios::ate | std::ios::binary);
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open file: " + filePath);
	}

	size_t fileSize = static_cast<size_t>(file.tellg());
	std::vector<uint8_t> buffer(fileSize / sizeof(uint32_t));
	file.seekg(0);
	file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
	file.close();

	return buffer;
}



vk::UniqueShaderModule Imp::Render::ShaderLoader::CreateShaderModule(const std::vector<uint32_t>& code,
																	 const Device& device)
{
	vk::ShaderModuleCreateInfo createInfo{
			{},
			code.size() * sizeof(uint32_t),
			code.data()
	};

	return device.getLogical().createShaderModuleUnique(createInfo);
}

vk::UniqueShaderModule Imp::Render::ShaderLoader::CreateShaderModule(const Device& device,
																			   const std::string& shaderName, vkutil::ShaderStage kind)
{

	std::string stageExtension;
	vk::ShaderStageFlagBits stageFlag;
	switch (kind) {
	case vkutil::ShaderStage::Vertex:
		stageExtension = ".vert.spv";
		stageFlag = vk::ShaderStageFlagBits::eVertex;
		break;
	case vkutil::ShaderStage::Fragment:
		stageExtension = ".frag.spv";
		stageFlag = vk::ShaderStageFlagBits::eFragment;
		break;
	case vkutil::ShaderStage::Compute:
		stageExtension = ".comp.spv";
		stageFlag = vk::ShaderStageFlagBits::eCompute;
		break;
	case vkutil::ShaderStage::Geometry:
		stageExtension = ".geom.spv";
		stageFlag = vk::ShaderStageFlagBits::eGeometry;
		break;
	default:
		throw std::runtime_error("Unknown shader stage");
	}

	std::filesystem::path filePath("shaders/SPIRV");
	filePath /= (shaderName + stageExtension);
	//std::cout << "Reading shader from: " << filePath << std::endl; // Debug statement

	if (!std::filesystem::exists(filePath)) {
		Debug::Throw("Shader file not found: {}", filePath.string());
	}
	const auto code = readFile(filePath.string());
	return CreateShaderModule(code, device);

}

//std::vector<std::pair<Imp::Render::vkutil::ShaderStage, vk::UniqueShaderModule>> Imp::Render::ShaderLoader::CreateShaderModules(const Device& device,
//                                                                                                                                const std::string& shaderName)
//{
//	FileDirectoryHelper fileDir;
//	fileDir.gotoChildDirectory("shaders/SPIRV");
//	std::vector<std::pair<vkutil::ShaderStage, vk::UniqueShaderModule>> shaderModules;
//
//	fileDir.
//
//};
