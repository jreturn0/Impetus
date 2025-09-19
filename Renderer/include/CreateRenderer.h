#pragma once
#include "Renderer.h"
namespace imp::gfx
{
    // Factory function to create a Renderer instance
    std::unique_ptr<Renderer> CreateRenderer(std::string_view windowTitle);
}
