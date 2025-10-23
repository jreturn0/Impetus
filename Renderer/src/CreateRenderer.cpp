#include "CreateRenderer.h"
#include "VKRenderer.h"
#include "utils/shader/ShaderCompiler.h"
#include "ConfigSystem.h"

namespace {
    utl::ConfigValueRef<std::string> cfg_shaderDir("assets.shader_directory", "assets/Shaders");
}
std::unique_ptr<imp::gfx::Renderer> imp::gfx::CreateRenderer(std::string_view windowTitle)
{
    VULKAN_HPP_DEFAULT_DISPATCHER.init();
    imp::gfx::ShaderCompiler::Instance().CheckCompileAll(cfg_shaderDir.get());
    return std::make_unique<VKRenderer>(windowTitle);
}
