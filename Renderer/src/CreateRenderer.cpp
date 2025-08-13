﻿#include "CreateRenderer.h"
#include "VKRenderer.h"
#include "utils/shader/ShaderCompiler.h"

std::unique_ptr<Imp::Render::Renderer> Imp::Render::CreateRenderer(uint32_t width, uint32_t height, const char* title,
                                                                   const std::string& shaderName)
{
	VULKAN_HPP_DEFAULT_DISPATCHER.init();
	Imp::Render::ShaderCompiler::Instance().CheckCompileAll();
	return std::make_unique<VKRenderer>(title,width, height );
}
