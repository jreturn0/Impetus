#include "pch.h"
#include "Renderer.h"

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>


Imp::Render::Renderer::Renderer(): imageIndex(0), sceneData({ 1.f }, { 1.f }, { 1.f }, { 0.25f,0.25f,0.25f,1.f }, { 0.5f,0.5f,0.f,1.f }, { 1.f,1.f,1.f,1.f })
{
}

Imp::Render::Renderer::~Renderer()
{
	

	
}

