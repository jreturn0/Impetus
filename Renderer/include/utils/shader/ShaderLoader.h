#pragma once
#include "utils/QuickMacros.h"
#include "utils/VKCommon.hpp"

namespace imp::gfx {
    namespace ShaderLoader {
        // Reads file and returns its contents as a vector of 32-bit unsigned integers
        std::vector<uint32_t> readFile(const std::string& filePath);
        // Reads file and returns its contents as a vector of 8-bit unsigned integers
        std::vector<uint8_t> readFile8(const std::string& filePath);
        // Creates a Vulkan shader module from given SPIR-V code
        vk::raii::ShaderModule CreateShaderModule(const std::vector<uint32_t>& code, const vk::raii::Device& device);
        // Creates a Vulkan shader module from a shader file
        vk::raii::ShaderModule CreateShaderModule(const vk::raii::Device& device, std::string_view shaderName, vkutil::ShaderStage kind = vkutil::ShaderStage::None);

    };
}
