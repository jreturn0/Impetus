//#include "Renderer.h"
//#include "shaderc/shaderc.hpp"
//#include <fstream>
//#include <sstream>
//#include <stdexcept>
//#include <string>
//
//std::string readFile(const std::string& filePath)
//{
//	std::ifstream file(filePath);
//	if (!file.is_open()) {
//		throw std::runtime_error("Failed to open file: " + filePath);
//	}
//
//	std::stringstream buffer;
//	buffer << file.rdbuf();
//	return buffer.str();
//}
//
//std::vector<uint32_t> OldImp::vkutil::CompileShader(const std::string& source, ShaderStage kind,
//	const std::string& shaderName)
//{
//	shaderc::Compiler compiler;
//	shaderc::CompileOptions options;
//
//	// Enable optimizations
//	options.SetOptimizationLevel(shaderc_optimization_level_performance);
//	auto fileSource = readFile(source);
//	// Compile the shader
//	const shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(fileSource, static_cast<shaderc_shader_kind>(kind), shaderName.c_str(), options);
//
//	// Check for compilation errors
//	if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
//		throw std::runtime_error(result.GetErrorMessage());
//	}
//
//	// Return the compiled SPIR-V bytecode as a vector of uint32_t
//	return { result.cbegin(), result.cend() };
//}
