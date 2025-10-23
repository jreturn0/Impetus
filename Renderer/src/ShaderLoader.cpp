#include "ConfigSystem.h"
#include "Debug.h"
#include "FSNavigator.h"
#include "utils/shader/ShaderLoader.h"
#include <filesystem>
#include <fstream>

namespace {
    utl::ConfigValueRef<std::string> cfg_spirvDir("assets.shader_spirv_directory", "assets/shaders/SPIRV");
}

std::vector<uint32_t> imp::gfx::ShaderLoader::readFile(const std::string& filePath)
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

std::vector<uint8_t> imp::gfx::ShaderLoader::readFile8(const std::string& filePath)
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



vk::raii::ShaderModule imp::gfx::ShaderLoader::CreateShaderModule(const std::vector<uint32_t>& code,
                                                                     const vk::raii::Device& device)
{
    vk::ShaderModuleCreateInfo createInfo{
            {},
            code.size() * sizeof(uint32_t),
            code.data()
    };

    return device.createShaderModule(createInfo);
}

vk::raii::ShaderModule imp::gfx::ShaderLoader::CreateShaderModule(const vk::raii::Device& device, std::string_view shaderName, vkutil::ShaderStage kind)
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
        Debug::FatalError("Unknown shader stage");
    }
    
    std::filesystem::path filePath(cfg_spirvDir.get());
    filePath /= (shaderName);
    filePath += stageExtension;
    //std::cout << "Reading shader from: " << filePath << std::endl; // Debug statement

    if (!std::filesystem::exists(filePath)) {        
        std::string filePathStr = filePath.string();
        Debug::Throw("Shader file not found: {}", filePathStr);
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
