#pragma once


#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"

namespace imp::gfx {
	namespace ShaderLoader {
		std::vector<uint32_t> readFile(const std::string& filePath);

		std::vector<uint8_t> readFile8(const std::string& filePath);


		
		vk::raii::ShaderModule CreateShaderModule(const std::vector<uint32_t>& code, const vk::raii::Device& device);


		vk::raii::ShaderModule CreateShaderModule(const vk::raii::Device& device, std::string_view shaderName, vkutil::ShaderStage kind = vkutil::ShaderStage::None);

	};
}
