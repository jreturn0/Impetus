﻿#include "utils/shader/ShaderCompiler.h"
#include "utils/shader/ShaderReflect.h"

#include <bitset>
#include <iostream>
#include <shaderc/shaderc.hpp>
#include <fstream>
#include <unordered_map>

#include "Debug.h"
#include "FSNavigator.h"
namespace {
    class FileSystemIncluder : public shaderc::CompileOptions::IncluderInterface
    {
    public:
        shaderc_include_result* GetInclude(const char* requested_source,
            shaderc_include_type type,
            const char* requesting_source,
            size_t include_depth) override
        {
            //std::cout << "GetInclude called for: " << requested_source << " from: " << requesting_source << std::endl;

            std::string resolved_path = ResolvePath(requested_source, requesting_source);
            //std::cout << "Resolved include path: " << resolved_path << std::endl;

            std::ifstream file(resolved_path);
            if (!file.is_open()) {
                Debug::FatalError("Failed to open include file: {}", resolved_path);
                return CreateShadercIncludeResult(requested_source, "", "File not found");
            }

            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string content = buffer.str();
            //std::cout << "Including file content: " << std::endl << content << std::endl;

            return CreateShadercIncludeResult(resolved_path, content);
        }

        void ReleaseInclude(shaderc_include_result* data) override
        {
            delete[] data->source_name;
            delete[] data->content;
            delete data;
        }

    private:
        std::string ResolvePath(const std::string& requested_source, const std::string& requesting_source)
        {
            std::filesystem::path requesting_path(requesting_source);
            std::filesystem::path include_path = requesting_path.parent_path() / requested_source;
            return include_path.string();
        }

        shaderc_include_result* CreateShadercIncludeResult(const std::string& source_name,
            const std::string& source_content,
            const std::string& error_message = "")
        {
            auto* result = new shaderc_include_result;
            result->source_name = copyString(source_name);
            result->content = copyString(source_content);

            result->source_name_length = source_name.size();
            result->content_length = source_content.size();
            result->user_data = nullptr;
            return result;
        }

        char* copyString(const std::string& str)
        {
            char* copy = new char[str.size() + 1];
            std::copy(str.begin(), str.end(), copy);
            copy[str.size()] = '\0';
            return copy;
        }
    };
}

imp::gfx::ShaderCompiler::ShaderCompiler()
{
}

imp::gfx::ShaderCompiler::~ShaderCompiler()
{
}
std::string imp::gfx::ShaderCompiler::readFile(const std::string& filePath)
{
    std::ifstream file(filePath, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filePath);
    }

    size_t fileSize = (size_t)file.tellg();
    std::string buffer(fileSize, ' ');
    file.seekg(0);
    file.read(&buffer[0], fileSize);
    file.close();

    return buffer;
}



bool imp::gfx::ShaderCompiler::ShouldCompileSPIRV(FSNavigator fileDir, const std::filesystem::path file)
{

    fileDir.goToFolder("SPIRV");
    std::string nameSPIRV = std::string((fileDir.getCurrentPath() / file.filename()).string()).append(".spv");
    bool compile = false;
    if (!fileDir.containsFile(nameSPIRV))
        compile = true;
    else
        try {
        compile = std::filesystem::last_write_time(file) > std::filesystem::last_write_time((nameSPIRV));
    }
    catch (std::filesystem::filesystem_error error) {
        Debug::Error("Error checking file times: {}", error.what());
    }
    fileDir.goToParent();

    return compile;
}





std::vector<uint32_t> imp::gfx::ShaderCompiler::CompileShader(const std::filesystem::path file, vkutil::ShaderStage kind)
{
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;

    // Enable optimizations
    options.SetOptimizationLevel(shaderc_optimization_level_performance);
    options.SetIncluder(std::make_unique<FileSystemIncluder>());
    auto path = file.string() + file.filename().string();
    //std::cout << path;
    auto fileSource = readFile(file.string());  // Use readFile to read the content of the shader source file
    // Compile the shader

    const shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(fileSource, static_cast<shaderc_shader_kind>(kind), file.filename().string().c_str(), options);
    try {
        if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
            std::cerr << result.GetErrorMessage();
            throw std::runtime_error(result.GetErrorMessage());
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what();
    }


    // Return the compiled SPIR-V bytecode as a vector of uint32_t
    return { result.cbegin(), result.cend() };
}

void imp::gfx::ShaderCompiler::SaveShader(const std::vector<uint32_t>& spvCode, const std::string& filePath)
{
    std::ofstream outFile(filePath, std::ios::binary | std::ios::out | std::ios::trunc);
    if (!outFile.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + filePath);
    }

    outFile.write(reinterpret_cast<const char*>(spvCode.data()), spvCode.size() * sizeof(uint32_t));
}




static imp::gfx::vkutil::ShaderStage GetStage(const std::filesystem::path& path)
{
    std::string extension = path.extension().string();

    if (extension == ".vert") {
        return imp::gfx::vkutil::ShaderStage::Vertex;
    }  if (extension == ".frag") {
        return imp::gfx::vkutil::ShaderStage::Fragment;
    }  if (extension == ".comp") {
        return imp::gfx::vkutil::ShaderStage::Compute;
    }  if (extension == ".geom") {
        return imp::gfx::vkutil::ShaderStage::Geometry;
    }
    return imp::gfx::vkutil::ShaderStage::None;
}



void imp::gfx::ShaderCompiler::CheckCompileAll(const std::string& dir)
{
    FSNavigator fileDir(dir);
    if (!fileDir.containsFolder("SPIRV")) {
        std::filesystem::create_directory(fileDir.getCurrentPath() / "SPIRV");
        fileDir.update();
    }

    for (const auto& file : fileDir.getFiles()) {
        auto file_ = fileDir.getCurrentPath() / file;
        vkutil::ShaderStage stage = GetStage(file);
        //std::cout << file.string() << std::endl;
        if (stage == vkutil::ShaderStage::None)
            continue;
        std::string nameSPIRV = std::string((fileDir.getCurrentPath() / std::string("SPIRV/") / file.filename()).string()).append(".spv");
        if (ShouldCompileSPIRV(fileDir, file_)) {
            auto spvCode = CompileShader(file_, stage);
            // Ensure the path is correctly constructed
            std::filesystem::path savePath = fileDir.getCurrentPath() / "SPIRV" / file.filename();
            std::string savePathStr = savePath.string() + ".spv";

            SaveShader(spvCode, savePathStr);
            //Debug::Info("Compiled and saved to SPIRV folder: {}", savePathStr);
            //std::cout << file.filename().string() + " compiled and saved to SPIRV folder.\n";
        }
        else {
            //std::cout << file.filename().string() + " is up to date.\n";
        }

        ReflectShader(readSPVFile(nameSPIRV));
    }
}



void imp::gfx::ShaderCompiler::CheckCompileShader(const std::string& name, const std::string& dir)
{
    FSNavigator fileDir(dir);
    if (!fileDir.containsFolder("SPIRV")) {
        std::filesystem::create_directory(fileDir.getCurrentPath() / "SPIRV");
        fileDir.update();
    }

    std::bitset<4> compileFlags;

    std::string filename = name + ".vert";
    if (ShouldCompileSPIRV(fileDir, filename)) {
        compileFlags.set(0);
        //Debug::Info("{} is out of date", filename );
        //std::cout << filename << " is out of date\n";
    }
    else {
        //std::cout << filename << " is up to date\n";
    }

    filename = name + ".frag";
    if (ShouldCompileSPIRV(fileDir, filename)) {
        compileFlags.set(1);
        //std::cout << filename << " is out of date\n";
    }
    else {
        //std::cout << filename << " is up to date\n";
    }


}