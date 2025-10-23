#pragma once
#include "utils/QuickMacros.h"
#include "utils/VKCommon.hpp"
#include <bitset>
#include <filesystem>
#include <string>
#include <vector>

class FSNavigator;

namespace imp::gfx
{
    // Singleton class to manage shader compilation
    class ShaderCompiler
    {

    public:
        ShaderCompiler(const ShaderCompiler&) = delete; 
        ShaderCompiler& operator=(const ShaderCompiler&) = delete;
        ShaderCompiler(ShaderCompiler&&) = delete;
        ShaderCompiler& operator=(ShaderCompiler&&) = delete;

        static ShaderCompiler& Instance()
        {
            static ShaderCompiler instance;
            return instance;
        }

        //checks and compiles all shaders in given directory that need compilation(no spv, out of date, etc)
        void CheckCompileAll(const std::string& dir = "assets/Shaders/");
        void CheckCompileShader(const std::string& name, const std::string& dir = "assets/Shaders/");
    private:
        ShaderCompiler();
        ~ShaderCompiler();

        std::string readFile(const std::string& filePath);
        bool ShouldCompileSPIRV(FSNavigator fileDir, const std::filesystem::path file);
        std::vector<uint32_t> CompileShader(const std::filesystem::path file, vkutil::ShaderStage kind);
        void SaveShader(const std::vector<uint32_t>& spirvCode, const std::string& filePath);
    };


}
