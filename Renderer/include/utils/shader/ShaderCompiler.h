#pragma once

#include <bitset>
#include <string>
#include <vector>
#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"
#include <filesystem>

class FSNavigator;

namespace imp::gfx
{

	class ShaderCompiler
	{
	private:
		ShaderCompiler();
		~ShaderCompiler();
		std::string readFile(const std::string& filePath);
		//bool ShouldCompileSPIRV(FileDirectoryHelper& fileDir, const std::string& name);
		bool ShouldCompileSPIRV(FSNavigator fileDir, const std::filesystem::path file);
		std::vector<uint32_t> CompileShader(const std::filesystem::path file, vkutil::ShaderStage kind);
		void SaveShader(const std::vector<uint32_t>& spirvCode, const std::string& filePath);
	public:
		DISABLE_COPY_AND_MOVE(ShaderCompiler);
		static ShaderCompiler& Instance()
		{
			static ShaderCompiler instance;
			return instance;
		}

		//check all shaders in given directory and compile and save what's needed
		void CheckCompileAll(const std::string& dir = "Shaders");
		void CheckCompileShader(const std::string& name, const std::string& dir = "Shaders");
	};


}
