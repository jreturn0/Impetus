#include "CreateRenderer.h"
#include "VKRenderer.h"
#include "utils/shader/ShaderCompiler.h"

std::unique_ptr<imp::gfx::Renderer> imp::gfx::CreateRenderer(std::string_view windowTitle)
{
    VULKAN_HPP_DEFAULT_DISPATCHER.init();
    imp::gfx::ShaderCompiler::Instance().CheckCompileAll();
    return std::make_unique<VKRenderer>(windowTitle);
}
