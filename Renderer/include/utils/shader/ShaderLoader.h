#pragma once


#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"

namespace Imp::Render {
	class Device;

	namespace ShaderLoader {
		std::vector<uint32_t> readFile(const std::string& filePath);

		std::vector<uint8_t> readFile8(const std::string& filePath);


		vk::UniqueShaderModule CreateShaderModule(const std::vector<uint32_t>& code,
												  const Device& device);



		vk::UniqueShaderModule CreateShaderModule(const Device& device,
																		  const std::string& shaderName, vkutil::ShaderStage kind = vkutil::ShaderStage::None);
		//std::vector<std::pair<vkutil::ShaderStage, vk::UniqueShaderModule>> CreateShaderModules(const Device& device,
		//													const std::string& shaderName);
	};
}
