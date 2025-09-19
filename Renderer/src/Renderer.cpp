#include "Renderer.h"

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>


imp::gfx::Renderer::Renderer(): m_imageIndex(0), m_sceneData({ 1.f }, { 1.f }, { 1.f }, { 0.25f,0.25f,0.25f,1.f }, { 0.5f,0.5f,0.f,1.f }, { 1.f,1.f,1.f,1.f })
{
}

imp::gfx::Renderer::~Renderer()
{
	

	
}

