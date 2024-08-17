#pragma once
#include "Renderer.h"
namespace Imp::Render
{
	std::unique_ptr<Renderer> CreateRenderer(uint32_t width, uint32_t height, const char* title, const std::string& shaderName);
	
}
